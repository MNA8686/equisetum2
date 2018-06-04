#include "Node.hpp"

#include <list>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

using namespace Equisetum2;

/*
最初にインデックス番号0の特別なrootノードが作成される。
親を設定していない、または親が消滅した全てのノードはrootノードを参照する。
親を持っていないのはrootノードのみである。
*/


Node::Node()
{
}

Node::~Node()
{
}

bool Node::Init(std::shared_ptr<Node> pNode, const String& name)
{
	auto pNodePool = Singleton<NodePool>::GetInstance();
	NodeID id = -1;

	// 未使用ノードあり？
	if (pNodePool->m_queFreeNode.size() > 0)
	{
		// 空き領域を取得
		id = pNodePool->m_queFreeNode.front();
		pNodePool->m_queFreeNode.pop();

		// ノードプール内の使用する領域を決定
		pNodePool->m_vNodeSlot[id] = pNode;
	}
	else
	{
		// 新しいIDを発行する
		id = static_cast<NodeID>(pNodePool->m_vNodeSlot.size());
		pNodePool->m_vNodeSlot.push_back(pNode);
	}

	// ノードのIDを設定
	pNode->SetID(id);
	// ノードの名前を設定
	pNode->SetName(name);
	// rootノードではない？
	if (id != 0)
	{
		// rootノードを親に設定
		pNode->SetParent(pNodePool->m_vNodeSlot[0]);
	}

	// 使用中のオブジェクト数を加算
	pNodePool->m_numOfObjects++;

	Logger::OutputDebug("spawn id %d, numOfObjects %d", id, pNodePool->m_numOfObjects);

	return true;
}

std::shared_ptr<Node> Node::Create(const String& objectName)
{
	if (auto tmpNode = std::make_shared<Node>())
	{
		if (Init(tmpNode, objectName))
		{
			return tmpNode;
		}
	}

	return nullptr;
}

bool Node::Visit(std::shared_ptr<Node> beginNode, const std::function<bool(std::shared_ptr<Node>&, int32_t)>& cb, int32_t nestDepth)
{
	if (!cb(beginNode, nestDepth))
	{
		return false;
	}

	if (beginNode->GetChildCount() > 0)
	{
		const std::vector<std::shared_ptr<Node>> children = beginNode->GetChildren();
		for (auto& child : children)
		{
			if (!Visit(child, cb, nestDepth + 1))
			{
				return false;
			}
		}
	}

	return true;
}

void Node::Dump()
{
	auto pNodePool = Singleton<NodePool>::GetInstance();

	if (pNodePool->m_numOfObjects == 0)
	{
		Logger::OutputInfo("--- node not found ---");
		return;
	}

	Logger::OutputInfo("--- dump begin numOfObjects %d ---", pNodePool->m_numOfObjects);

	Visit(pNodePool->m_vNodeSlot[0], [](std::shared_ptr<Node>& node, int32_t nestDepth)->bool {

		String out;

		for (int i = 0; i < nestDepth; i++)
		{
			out += "  ";
		}

		std::shared_ptr<Node> parent = node->GetParent();

		out += String::Sprintf("obj index %d, children num %d, parent index %d, name '%s'", node->GetID(), node->GetChildCount(), parent ? parent->GetID() : -1, node->GetName().c_str());

		Logger::OutputInfo(out.c_str());

		return true;
	});

	Logger::OutputInfo("vGcQueue %d, queFreeNode %d, listZombie %d", pNodePool->m_vGcQueue.size(), pNodePool->m_queFreeNode.size(), pNodePool->m_listZombie.size());
	Logger::OutputInfo("--- dump end ---");
}

void Node::GC()
{
	auto pNodePool = Singleton<NodePool>::GetInstance();

	// 削除対象リストを元にオブジェクトの削除を行う
	for (auto& nodeID : pNodePool->m_vGcQueue)
	{
		pNodePool->m_listZombie.push_back(pNodePool->m_vNodeSlot[nodeID]);	// ゾンビ検出用

		pNodePool->m_vNodeSlot[nodeID].reset();
		pNodePool->m_numOfObjects--;

		pNodePool->m_queFreeNode.push(nodeID);
	}

	// 削除対象リストクリア
	pNodePool->m_vGcQueue.clear();

	// ゾンビ検出を行う
	pNodePool->m_listZombie.remove_if([](std::weak_ptr<Node>& node)->bool {
		return node.expired();
	});
	if (!pNodePool->m_listZombie.empty())
	{
		Logger::OutputError("zombie num %d", pNodePool->m_listZombie.size());
	}
}

void Node::DestroyThemAll()
{
	auto pNodePool = Singleton<NodePool>::GetInstance();

	for (auto& node : pNodePool->m_vNodeSlot)
	{
		// ルートノード以外なら破棄する
		if (node &&
			node->GetID() != 0)
		{
			node->Destroy();
		}
	}

	GC();

	Logger::OutputDebug("obj num %d, destroy %d, free %d, zombi %d", pNodePool->m_numOfObjects, pNodePool->m_vGcQueue.size(), pNodePool->m_queFreeNode.size(), pNodePool->m_listZombie.size());

	for (auto& zombie : pNodePool->m_listZombie)
	{
		if (auto node = zombie.lock())
		{
			Logger::OutputError("  zombi nodeID %d", node->GetID());
		}
	}
}

int32_t Node::NumOfObjects()
{
	auto pNodePool = Singleton<NodePool>::GetInstance();

	return pNodePool->m_numOfObjects;
}

NodeID Node::GetID() const
{
	return m_nodeID;
}

void Node::SetID(NodeID id)
{
	m_nodeID = id;
}

void Node::Destroy()
{
	auto pNodePool = Singleton<NodePool>::GetInstance();

	if (!m_destroyed)
	{
		// 削除フラグを設定
		m_destroyed = true;

		// 親の参照リストから自分を削除
		DetachParent();

		// 全ての子をデタッチ
		DetachChildren();

		// 削除キューに入れる(後のGCでこのオブジェクトは破棄される)
		pNodePool->m_vGcQueue.push_back(m_nodeID);
	}
}

bool Node::IsDestroyed() const
{
	return m_destroyed;
}

void Node::DetachParent()
{
	auto pNodePool = Singleton<NodePool>::GetInstance();

	if (m_parentId < 0)
	{
		return;
	}

	// 親のインスタンスが存在する？
	if (std::shared_ptr<Node> pParent = pNodePool->m_vNodeSlot[m_parentId])
	{
		// 破棄されていない？
		if (!pParent->IsDestroyed())
		{
			// 親の子リストから自分を削除する
			auto& refList = pParent->m_listChildren;

			auto it = refList.begin();
			while (it != refList.end())
			{
				if (*it == m_nodeID)
				{
					refList.erase(it);
					break;
				}
				++it;
			}
		}

		m_parentId = -1;
	}
}

void Node::SetParent(std::shared_ptr<Node> newParent)
{
	auto pNodePool = Singleton<NodePool>::GetInstance();

	if (m_nodeID == 0)
	{
		Logger::OutputError(u8"rootノードに親を設定しようとしました。");
		return;
	}

	if (!newParent)
	{
		// rootノードを取得
		newParent = pNodePool->m_vNodeSlot[0];
	}

	if (!newParent->IsDestroyed())
	{
		// 前の親の子リストから自分を削除
		DetachParent();

		// 新しい親の子リストに追加
		newParent->m_listChildren.push_back(m_nodeID);

		// 新しい親をセット
		m_parentId = newParent->GetID();

		Logger::OutputDebug("obj %d, new parent %d", m_nodeID, newParent->GetID());
	}
}

std::shared_ptr<Node> Node::GetParent() const
{
	auto pNodePool = Singleton<NodePool>::GetInstance();

	if (m_nodeID > 0)
	{
		if (std::shared_ptr<Node> pParent = pNodePool->m_vNodeSlot[m_parentId])
		{
			if (!pParent->IsDestroyed())
			{
				return pParent;
			}
		}
	}

	return nullptr;
}

std::vector<std::shared_ptr<Node>> Node::GetChildren() const
{
	auto pNodePool = Singleton<NodePool>::GetInstance();

	std::vector<std::shared_ptr<Node>> vChildren;
	vChildren.reserve(m_listChildren.size());

	for (auto& nodeID : m_listChildren)
	{
		auto& pNode = pNodePool->m_vNodeSlot[nodeID];
		if (pNode &&
			!pNode->IsDestroyed())
		{
			vChildren.push_back(pNode);
		}
	}

	return vChildren;
}

int32_t Node::GetChildCount() const
{
	return static_cast<int32_t>(m_listChildren.size());
}

void Node::DetachChildren()
{
	auto pNodePool = Singleton<NodePool>::GetInstance();

	// 子を全て解除
	Logger::OutputDebug("obj %d, num children %d", m_nodeID, m_listChildren.size());

	for (auto& nodeID : m_listChildren)
	{
		auto& pNode = pNodePool->m_vNodeSlot[nodeID];

		if (pNode &&
			!pNode->IsDestroyed())
		{
			pNode->SetParent(nullptr);	// nullptrを設定すると自動的に親はrootノードに設定される

			Logger::OutputDebug("  nodeID %d, detach childID %d", m_nodeID, pNode->GetID());
		}
	}
}

bool Node::HasParent() const
{
	return m_parentId >= 0;
}

NodeID Node::GetParentID() const
{
	return m_parentId;
}

std::list<NodeID>& Node::GetChildrenID()
{
	return m_listChildren;
}

std::shared_ptr<Node>& Node::GetNodeByID(NodeID nodeID)
{
	auto pNodePool = Singleton<NodePool>::GetInstance();

	if (nodeID < 0 ||
		nodeID >= static_cast<NodeID>(pNodePool->m_vNodeSlot.size()) ||
		!pNodePool->m_vNodeSlot[nodeID])
	{
		// アサート
	}

	return pNodePool->m_vNodeSlot[nodeID];
}

void Node::SetName(const String & name)
{
	m_name = name;
}

String Node::GetName()
{
	return m_name;
}

