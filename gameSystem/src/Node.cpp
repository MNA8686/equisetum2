#include "Node.hpp"

#include <list>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

using namespace Equisetum2;

/*
�ŏ��ɃC���f�b�N�X�ԍ�0�̓��ʂ�root�m�[�h���쐬�����B
�e��ݒ肵�Ă��Ȃ��A�܂��͐e�����ł����S�Ẵm�[�h��root�m�[�h���Q�Ƃ���B
�e�������Ă��Ȃ��̂�root�m�[�h�݂̂ł���B
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

	// ���g�p�m�[�h����H
	if (pNodePool->m_queFreeNode.size() > 0)
	{
		// �󂫗̈���擾
		id = pNodePool->m_queFreeNode.front();
		pNodePool->m_queFreeNode.pop();

		// �m�[�h�v�[�����̎g�p����̈������
		pNodePool->m_vNodeSlot[id] = pNode;
	}
	else
	{
		// �V����ID�𔭍s����
		id = static_cast<NodeID>(pNodePool->m_vNodeSlot.size());
		pNodePool->m_vNodeSlot.push_back(pNode);
	}

	// �m�[�h��ID��ݒ�
	pNode->SetID(id);
	// �m�[�h�̖��O��ݒ�
	pNode->SetName(name);
	// root�m�[�h�ł͂Ȃ��H
	if (id != 0)
	{
		// root�m�[�h��e�ɐݒ�
		pNode->SetParent(pNodePool->m_vNodeSlot[0]);
	}

	// �g�p���̃I�u�W�F�N�g�������Z
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

	// �폜�Ώۃ��X�g�����ɃI�u�W�F�N�g�̍폜���s��
	for (auto& nodeID : pNodePool->m_vGcQueue)
	{
		pNodePool->m_listZombie.push_back(pNodePool->m_vNodeSlot[nodeID]);	// �]���r���o�p

		pNodePool->m_vNodeSlot[nodeID].reset();
		pNodePool->m_numOfObjects--;

		pNodePool->m_queFreeNode.push(nodeID);
	}

	// �폜�Ώۃ��X�g�N���A
	pNodePool->m_vGcQueue.clear();

	// �]���r���o���s��
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
		// ���[�g�m�[�h�ȊO�Ȃ�j������
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
		// �폜�t���O��ݒ�
		m_destroyed = true;

		// �e�̎Q�ƃ��X�g���玩�����폜
		DetachParent();

		// �S�Ă̎q���f�^�b�`
		DetachChildren();

		// �폜�L���[�ɓ����(���GC�ł��̃I�u�W�F�N�g�͔j�������)
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

	// �e�̃C���X�^���X�����݂���H
	if (std::shared_ptr<Node> pParent = pNodePool->m_vNodeSlot[m_parentId])
	{
		// �j������Ă��Ȃ��H
		if (!pParent->IsDestroyed())
		{
			// �e�̎q���X�g���玩�����폜����
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
		Logger::OutputError(u8"root�m�[�h�ɐe��ݒ肵�悤�Ƃ��܂����B");
		return;
	}

	if (!newParent)
	{
		// root�m�[�h���擾
		newParent = pNodePool->m_vNodeSlot[0];
	}

	if (!newParent->IsDestroyed())
	{
		// �O�̐e�̎q���X�g���玩�����폜
		DetachParent();

		// �V�����e�̎q���X�g�ɒǉ�
		newParent->m_listChildren.push_back(m_nodeID);

		// �V�����e���Z�b�g
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

	// �q��S�ĉ���
	Logger::OutputDebug("obj %d, num children %d", m_nodeID, m_listChildren.size());

	for (auto& nodeID : m_listChildren)
	{
		auto& pNode = pNodePool->m_vNodeSlot[nodeID];

		if (pNode &&
			!pNode->IsDestroyed())
		{
			pNode->SetParent(nullptr);	// nullptr��ݒ肷��Ǝ����I�ɐe��root�m�[�h�ɐݒ肳���

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
		// �A�T�[�g
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

