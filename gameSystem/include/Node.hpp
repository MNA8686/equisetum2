#if !defined(_EQNODE_H_)
#define _EQNODE_H_

#include "Equisetum2.h"
#include "INodeAttachment.hpp"
#include "EqVector.hpp"
using namespace Equisetum2;

//#include <queue>
#include <vector>
//#include <list>
#include <functional>

#if 0
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/list.hpp>
#include <cereal/types/queue.hpp>
#endif

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
class Node final
{
public:

	Node() = default;
	~Node() = default;
	//virtual ~Node() = default;

#if 0
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
#endif
	static NodeID GetFreeNodeWithInit(const String& objectName)
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();
		auto ctx = pNodePool->GetContext();
		//int32_ index = 0;

		// 未使用ノードあり？
		if (ctx->m_queFreeNode.Size() == 0)
		{
			return -1;
		}

		NodeID id = ctx->m_queFreeNode.Back();
		ctx->m_queFreeNode.PopBack();

		Node<T>& node = ctx->m_vNodeSlot[id];
		
		// ノードのIDを設定
		node.SetID(id);
		node.m_attach.SetNodeID(id);
		// ノードの名前を設定
		node.SetName(objectName);
		// rootノードではない？
		if (id != 0)
		{
			// rootノードを親に設定
			node.SetParent(0);
		}

		// 使用中のオブジェクト数を加算
		ctx->m_numOfObjects++;

		// 再構築フラグセット
//		pNodePool->m_dirty = true;

		Logger::OutputDebug("spawn id %d, numOfObjects %d", id, ctx->m_numOfObjects);

		return id;
	}

#if 0
	void SetAttach(std::shared_ptr<T>& attach)
	{
		m_attach = attach;
		m_attach->SetNodeID(GetID());
	}
#endif

	//std::shared_ptr<T>& GetAttach()
	T& GetAttach()
	{
		return m_attach;
	}

	void Destroy()
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();
		//auto pContext = pNodePool->GetContext();

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

	//void SetParent(std::shared_ptr<Node<T>> newParent)
	void SetParent(NodeID newParent)
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();
		auto ctx = pNodePool->GetContext();

		if (m_nodeID == 0)
		{
			Logger::OutputError(u8"rootノードに親を設定しようとしました。");
			return;
		}

		//if (!newParent)
		if (newParent < 0)
		{
			// rootノードを取得
			newParent = 0;// pContext->m_vNodeSlot[0].GetID();
		}

		auto& newParentNode = ctx->m_vNodeSlot[newParent];
		//if (!newParent->IsDestroyed())
		if (!newParentNode.IsDestroyed())
		{
			// 前の親の子リストから自分を削除
			DetachParent();

			// 新しい親の子リストに追加
			newParentNode.m_listChildren.PushBack(m_nodeID);

			// 新しい親をセット
			m_parentId = newParent;// newParent->GetID();

			//Logger::OutputDebug("obj %d, new parent %d", m_nodeID, newParent->GetID());
			Logger::OutputDebug("obj %d, new parent %d", m_nodeID, newParentNode.GetID());

			// 再構築フラグセット
			//pNodePool->m_dirty = true;
		}
	}

	//std::shared_ptr<Node<T>> GetParent() const
	NodeID GetParentID() const
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();
		auto ctx = pNodePool->GetContext();

		if (m_parentId >= 0)
		{
			auto& parentNode = ctx->m_vNodeSlot[m_parentId];
			if (!parentNode.IsDestroyed())
			{
				return m_parentId;
			}
		}

		return -1;
	}

	void DetachChildren()
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();
		auto ctx = pNodePool->GetContext();

		// 子を全て解除
		Logger::OutputDebug("obj %d, num children %d", m_nodeID, m_listChildren.Size());

		for (auto& nodeID : m_listChildren)
		{
			auto& node = ctx->m_vNodeSlot[nodeID];

			if(!node.IsDestroyed())
			{
				node.SetParent(-1);	// nullptrを設定すると自動的に親はrootノードに設定される

				Logger::OutputDebug("  nodeID %d, detach childID %d", m_nodeID, node.GetID());
			}
		}
	}

	//std::vector<std::shared_ptr<Node<T>>> GetChildren() const
	const EqVector<NodeID>& GetChildrenID() const
	{
		return m_listChildren;
#if 0
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
#endif
	}

	int32_t GetChildCount() const
	{
		return m_listChildren.Size();
		//return static_cast<int32_t>(m_listChildren.size());
	}

	bool HasParent() const
	{
		return m_parentId >= 0;
	}

	/*virtual*/ bool AddScheduler()
	{
		return true;
	}

	/*virtual*/ bool OnSchedule()
	{
		return true;
	}

	static void GC()
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();
		auto ctx = pNodePool->GetContext();

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
		//	pNodePool->m_listZombie.push_back(pNodePool->m_vNodeSlot[nodeID]);	// ゾンビ検出用

			ctx->m_vNodeSlot[nodeID] = {};		//TODO:解放処理は？
			ctx->m_numOfObjects--;

			ctx->m_queFreeNode.PushBack(nodeID);
		}

		// 削除対象リストクリア
		pNodePool->m_vGcQueue.clear();

#if 0
		// ゾンビ検出を行う
		pNodePool->m_listZombie.remove_if([](std::weak_ptr<Node<T>>& node)->bool {
			return node.expired();
		});
		if (!pNodePool->m_listZombie.empty())
		{
			Logger::OutputError("zombie num %d", pNodePool->m_listZombie.size());
		}
#endif
	}

	static void DestroyThemAll()
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();
		auto ctx = pNodePool->GetContext();

		for (auto& node : ctx->m_vNodeSlot)
		{
			if (node.m_nodeID > 0)
			{
				node.Destroy();
			}
		}

		GC();

		//Logger::OutputDebug("obj num %d, destroy %d, free %d, zombi %d", pNodePool->m_numOfObjects, pNodePool->m_vGcQueue.size(), pNodePool->m_queFreeNode.size(), pNodePool->m_listZombie.size());
		Logger::OutputDebug("obj num %d, destroy %d, free %d", ctx->m_numOfObjects, pNodePool->m_vGcQueue.size(), ctx->m_queFreeNode.Size());

#if 0
		for (auto& zombie : pNodePool->m_listZombie)
		{
			if (auto node = zombie.lock())
			{
				Logger::OutputError("  zombi nodeID %d", node->GetID());
			}
		}
#endif
	}

	static int32_t NumOfObjects()
	{
		auto ctx = Singleton<NodePool<T>>::GetInstance()->GetContext();
		return ctx->m_numOfObjects;
	}

	//static std::shared_ptr<Node<T>>& GetNodeByID(NodeID nodeID)
	static Node<T>& GetNodeByID(NodeID nodeID)
	{
		auto ctx = Singleton<NodePool<T>>::GetInstance()->GetContext();

#if 0
		if (nodeID < 0 ||
			nodeID >= static_cast<NodeID>(ctx.m_vNodeSlot.Size()) ||
			ctx.m_vNodeSlot[nodeID] < 0)
		{
			// アサート
		}
#endif

		return ctx->m_vNodeSlot[nodeID];
	}

#if 0
	const std::list<NodeID>& GetChildrenID() const
	{
		return m_listChildren;
	}
#endif

	// cbでfalseを返すとvisitを終了する
	//static bool Visit(std::shared_ptr<Node<T>> beginNode, const std::function<bool(std::shared_ptr<Node<T>>&, int32_t nestDepth)>& cb, int32_t nestDepth = 0)
	static bool Visit(Node<T>& beginNode, const std::function<bool(Node<T>&, int32_t nestDepth)>& cb, int32_t nestDepth = 0)
	{
		if (!cb)
		{
			return false;
		}

		if (cb(beginNode, nestDepth))
		{
			if (beginNode.GetChildCount() > 0)
			{
				auto ctx = Singleton<NodePool<T>>::GetInstance()->GetContext();

				//const std::list<NodeID> children = beginNode.GetChildren();
				for (auto& child : beginNode.GetChildrenID())
				{
					Visit(ctx->m_vNodeSlot[child], cb, nestDepth + 1);
				}
			}
		}

		return true;
	}

	static void Dump()
	{
#if 0
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
#endif
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

#if 0
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
#endif

protected:
#if 0
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
#endif

	void SetID(NodeID id)
	{
		m_nodeID = id;
	}

#if 0
	NodeID GetParentID() const
	{
		return m_parentId;
	}
#endif

	//std::shared_ptr<Node<T>>& Self()
	Node<T>& Self()
	{
		return GetNodeByID(GetID());
	}

private:

	// --- serialize begin ---
	NodeID m_nodeID = -1;				/// インスタンスのID、管理配列のインデックスと同じである
	NodeID m_parentId = -1;				/// 親ノードのID、管理配列のインデックスと同じである
	EqVector<NodeID> m_listChildren;	/// 子ノードIDリスト
	String m_name;						/// ノードの名前
	bool m_destroyed = false;			/// 破棄フラグ trueにするとGC対象となる
	T m_attach;							/// アタッチされたオブジェクト
	// --- serialize end ---
#if 0
	// --- serialize begin ---
	NodeID m_nodeID = -1;				/// インスタンスのID、管理配列のインデックスと同じである
	NodeID m_parentId = -1;				/// 親ノードのID、管理配列のインデックスと同じである
	std::list<NodeID> m_listChildren;	/// 子ノードIDリスト
	String m_name;						/// ノードの名前
	bool m_destroyed = false;			/// 破棄フラグ trueにするとGC対象となる
	std::shared_ptr<T> m_attach;		/// アタッチされたオブジェクト
	// --- serialize end ---
#endif

	void DetachParent()
	{
#if 1
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();
		auto ctx = pNodePool->GetContext();

		if (m_parentId < 0)
		{
			return;
		}

		// 親のインスタンスが存在する？
		Node<T>& parent = ctx->m_vNodeSlot[m_parentId];

		// 破棄されていない？
		if (!parent.IsDestroyed())
		{
			// 親の子リストから自分を削除する
			auto& refList = parent.m_listChildren;

			auto it = refList.begin();
			while (it != refList.end())
			{
				if (*it == m_nodeID)
				{
					refList.Erase(it);
					break;
				}
				++it;
			}

			m_parentId = -1;

			// 再構築フラグセット
//			pNodePool->m_dirty = true;
		}
#endif
	}
};

template<class T>
class NodePool final
{
public:
	friend Singleton<NodePool<T>>;	// シングルトンからインスタンスを作成してもらえるようにする

	static const int32_t defaultNum = 4096;

	typedef struct
	{
		Node<T> m_vNodeSlot[defaultNum];	// ノードの実体を格納した配列
		int32_t m_numOfObjects = 0;			// 現在生成されているノードの数
		EqVector<NodeID> m_queFreeNode;		// 未使用状態のノードのIDを格納したキュー
		//EqVector<NodeID> m_listZombie;		// ゾンビノードリスト
		Node<T> m_null;	// ヌルノード
	}Context;
	
	EqHeap::Container<Context> CreateContext()
	{
		auto handle = Singleton<EqHeap>::GetInstance()->New<Context>();
		if (handle)
		{
			if (auto ctx = handle.Ref())
			{
				ctx->m_queFreeNode.Resize(defaultNum);

				int32_t index = defaultNum - 1;
				for (auto& elem : ctx->m_queFreeNode)
				{
					elem = index;
					index--;
				}

//				ctx->m_null.m_nodeID = INodeAttachment::nullNode;
			}
		}

		return handle;
	}

	void SetContext(EqHeap::Container<Context>& ctx)
	{
		m_pCtx = ctx.Ref();
	}

#if 0
	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(CEREAL_NVP(m_vNodeSlot));
		archive(CEREAL_NVP(m_queFreeNode));
		archive(CEREAL_NVP(m_numOfObjects));
		archive(CEREAL_NVP(m_listZombie));
	}
#endif

#if 0
	void Reset()
	{
		m_vNodeSlot.clear();
		m_vNodeSlot.resize(defaultNum);
		
		m_vGcQueue.clear();
		m_vGcQueue.reserve(defaultNum);

	   std::queue<NodeID> empty;
	   std::swap( m_queFreeNode, empty );
   
		for (int i = 0; i < defaultNum; i++)
		{
			m_queFreeNode.push(i);
		}
	}
#endif

	Context* GetContext() { return m_pCtx; }
//	Context* operator ->() { return m_pCtx; }

protected:

	friend Node<T>;

#if 0
	// --- serialize begin ---
	std::vector<std::shared_ptr<Node<T>>> m_vNodeSlot;		// ノードの実体を格納した配列
	std::queue<NodeID> m_queFreeNode;					// 未使用状態のノードのIDを格納したキュー
	int32_t m_numOfObjects = 0;							// 現在生成されているノードの数
	std::list<std::weak_ptr<Node<T>>> m_listZombie;		// ゾンビノードリスト
	// --- serialize end ---
#endif

	Context* m_pCtx;

	std::vector<NodeID> m_vGcQueue;							// GC対象となるノードのIDを格納した配列
//	bool m_dirty = true;									// スケジュール配列の再構築が必要かどうか
//	std::vector<std::shared_ptr<Node<T>>> m_vNodeScheduler;	// スケジュール配列

	NodePool() = default;
	//{
	//	Reset();
	//}
};

#endif
