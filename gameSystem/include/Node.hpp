#if !defined(_EQNODE_H_)
#define _EQNODE_H_

#include "Equisetum2.h"
#include "INodeAttachment.hpp"
using namespace Equisetum2;

#include <queue>
#include <vector>
#include <list>
#include <functional>

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/list.hpp>

#if 0
using NodeID = int32_t;

class INodeAttachment
{
public:
	virtual void SetNodeID(NodeID id) = 0;
	virtual NodeID GetNodeID() const = 0;
};
#endif

template<class T>
class Node
{
public:

	Node() = default;
	virtual ~Node() = default;

	static std::shared_ptr<Node<T>> Create(const String& objectName)
	{
		if (auto tmpNode = std::make_shared<Node<T>>())
		{
			if (Init(tmpNode, objectName))
			{
				return tmpNode;
			}
		}

		return nullptr;
	}

	void SetAttach(std::shared_ptr<T> attach)
	{
		m_attach = attach;
		m_attach->SetNodeID(GetID());
	}

	std::shared_ptr<T>& GetAttach()
	{
		return m_attach;
	}

	void Destroy()
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();

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

			// 再構築フラグセット
			//pNodePool->m_dirty = true;
		}
	}

	bool IsDestroyed() const
	{
		return m_destroyed;
	}

	NodeID GetID() const
	{
		return m_nodeID;
	}

	void SetName(const String & name)
	{
		m_name = name;
	}

	String GetName()
	{
		return m_name;
	}

	void SetParent(std::shared_ptr<Node<T>> newParent)
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();

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

			// 再構築フラグセット
			//pNodePool->m_dirty = true;
		}
	}

	std::shared_ptr<Node<T>> GetParent() const
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();

		if (m_parentId > 0)
		{
			if (std::shared_ptr<Node<T>> pParent = pNodePool->m_vNodeSlot[m_parentId])
			{
				if (!pParent->IsDestroyed())
				{
					return pParent;
				}
			}
		}

		return nullptr;
	}

	void DetachChildren()
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();

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

	std::vector<std::shared_ptr<Node<T>>> GetChildren() const
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();

		std::vector<std::shared_ptr<Node<T>>> vChildren;
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

	int32_t GetChildCount() const
	{
		return static_cast<int32_t>(m_listChildren.size());
	}

	bool HasParent() const
	{
		return m_parentId >= 0;
	}

	virtual bool AddScheduler()
	{
		return true;
	}

	virtual bool OnSchedule()
	{
		return true;
	}

	static void GC()
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();

#if 0
		if (pNodePool->m_dirty)
		{
			// スケジュール配列を削除
			pNodePool->m_vNodeScheduler.clear();
		}
#endif

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
		pNodePool->m_listZombie.remove_if([](std::weak_ptr<Node<T>>& node)->bool {
			return node.expired();
		});
		if (!pNodePool->m_listZombie.empty())
		{
			Logger::OutputError("zombie num %d", pNodePool->m_listZombie.size());
		}
	}

	static void DestroyThemAll()
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();

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

	static int32_t NumOfObjects()
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();
		return pNodePool->m_numOfObjects;
	}

	static std::shared_ptr<Node<T>>& GetNodeByID(NodeID nodeID)
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();

		if (nodeID < 0 ||
			nodeID >= static_cast<NodeID>(pNodePool->m_vNodeSlot.size()) ||
			!pNodePool->m_vNodeSlot[nodeID])
		{
			// アサート
		}

		return pNodePool->m_vNodeSlot[nodeID];
	}

	std::list<NodeID>& GetChildrenID()
	{
		return m_listChildren;
	}

	// cbでfalseを返すとvisitを終了する
	static bool Visit(std::shared_ptr<Node<T>> beginNode, const std::function<bool(std::shared_ptr<Node<T>>&, int32_t nestDepth)>& cb, int32_t nestDepth = 0)
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();

		if (!cb(beginNode, nestDepth))
		{
			return false;
		}

		if (beginNode->GetChildCount() > 0)
		{
			const std::list<NodeID> children = beginNode->GetChildrenID();
			for (auto& child : children)
			{
				if (!Visit(pNodePool->m_vNodeSlot[child], cb, nestDepth + 1))
				{
					return false;
				}
			}
		}

		return true;
	}

	static void Dump()
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();

		if (pNodePool->m_numOfObjects == 0)
		{
			Logger::OutputInfo("--- node not found ---");
			return;
		}

		Logger::OutputInfo("--- dump begin numOfObjects %d ---", pNodePool->m_numOfObjects);

		Visit(pNodePool->m_vNodeSlot[0], [](std::shared_ptr<Node<T>>& node, int32_t nestDepth)->bool {

			String out;

			for (int i = 0; i < nestDepth; i++)
			{
				out += "  ";
			}

			std::shared_ptr<Node<T>> parent = node->GetParent();

			out += String::Sprintf("obj index %d, children num %d, parent index %d, name '%s'", node->GetID(), node->GetChildCount(), parent ? parent->GetID() : -1, node->GetName().c_str());

			Logger::OutputInfo(out.c_str());

			return true;
		});

		Logger::OutputInfo("vGcQueue %d, queFreeNode %d, listZombie %d", pNodePool->m_vGcQueue.size(), pNodePool->m_queFreeNode.size(), pNodePool->m_listZombie.size());
		Logger::OutputInfo("--- dump end ---");
	}

	static void MakeScheduler()
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();

#if 0
		if (pNodePool->m_dirty)
		{
			// スケジュール配列をクリア
			pNodePool->m_vNodeScheduler.clear();

			// ルートノード取得
			if (auto pThisNode = pNodePool->m_vNodeSlot[0])
			{
				// ノードを辿り、スケジュール配列に追加していく
				Visit(pThisNode, [](std::shared_ptr<Node<T>>& node, int32_t nestDepth)->bool {
					bool add = node->AddScheduler();		// 追加条件判定
					if (add)
					{
						// スケジュール配列にノードを追加
						Singleton<NodePool<T>>::GetInstance()->m_vNodeScheduler.push_back(node);
					}
					return add;
				});
			}

			pNodePool->m_dirty = false;
		}
#endif
	}

	static void ProcScheduler()
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();
#if 0
		for (auto& node : pNodePool->m_vNodeScheduler)
		{
			node->OnSchedule();
		}
#endif
	}

	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(CEREAL_NVP(m_nodeID));
		archive(CEREAL_NVP(m_parentId));
		archive(CEREAL_NVP(m_listChildren));
		archive(CEREAL_NVP(m_destroyed));
		archive(CEREAL_NVP(m_attach));

		std::string& m_name_str = m_name;
		archive(CEREAL_NVP(m_name_str));
	}

protected:
	static bool Init(std::shared_ptr<Node<T>> pNode, const String& name)
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();
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

		// 再構築フラグセット
//		pNodePool->m_dirty = true;

		Logger::OutputDebug("spawn id %d, numOfObjects %d", id, pNodePool->m_numOfObjects);

		return true;
	}

	void SetID(NodeID id)
	{
		m_nodeID = id;
	}

	NodeID GetParentID() const
	{
		return m_parentId;
	}

	std::shared_ptr<Node<T>>& Self()
	{
		return GetNodeByID(GetID());
	}

private:

	// --- serialize begin ---
	NodeID m_nodeID = -1;				/// インスタンスのID、管理配列のインデックスと同じである
	NodeID m_parentId = -1;				/// 親ノードのID、管理配列のインデックスと同じである
	std::list<NodeID> m_listChildren;	/// 子ノードIDリスト
	String m_name;						/// ノードの名前
	bool m_destroyed = false;			/// 破棄フラグ trueにするとGC対象となる
	std::shared_ptr<T> m_attach;		/// アタッチされたオブジェクト
	// --- serialize end ---

	void DetachParent()
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();

		if (m_parentId < 0)
		{
			return;
		}

		// 親のインスタンスが存在する？
		if (std::shared_ptr<Node<T>> pParent = pNodePool->m_vNodeSlot[m_parentId])
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

			// 再構築フラグセット
//			pNodePool->m_dirty = true;
		}
	}
};

template<class T>
class NodePool final
{
public:
	friend Singleton<NodePool<T>>;	// シングルトンからインスタンスを作成してもらえるようにする

	const int32_t defaultNum = 4096;

	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(CEREAL_NVP(m_vNodeSlot));
		archive(CEREAL_NVP(m_queFreeNode));
		archive(CEREAL_NVP(m_numOfObjects));
		archive(CEREAL_NVP(m_listZombie));
	}

protected:

	friend Node<T>;

	// --- serialize begin ---
	std::vector<std::shared_ptr<Node<T>>> m_vNodeSlot;		// ノードの実体を格納した配列
	std::queue<NodeID> m_queFreeNode;					// 未使用状態のノードのIDを格納したキュー
	int32_t m_numOfObjects = 0;							// 現在生成されているノードの数
	std::list<std::weak_ptr<Node<T>>> m_listZombie;		// ゾンビノードリスト
	// --- serialize end ---

	std::vector<NodeID> m_vGcQueue;							// GC対象となるノードのIDを格納した配列
//	bool m_dirty = true;									// スケジュール配列の再構築が必要かどうか
//	std::vector<std::shared_ptr<Node<T>>> m_vNodeScheduler;	// スケジュール配列

	NodePool()	// インスタンス作成封じ
	{
		m_vNodeSlot.resize(defaultNum);
		m_vGcQueue.reserve(defaultNum);

		for (int i = 0; i < defaultNum; i++)
		{
			m_queFreeNode.push(i);
		}
	}
};

#endif
