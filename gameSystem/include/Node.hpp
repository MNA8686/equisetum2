#if !defined(_EQNODE_H_)
#define _EQNODE_H_

#include "Equisetum2.h"
#include "INodeAttachment.hpp"
#include "EqVector.hpp"
using namespace Equisetum2;

#include <vector>
#include <functional>

template<class T>
class Node final
{
public:

	Node() = default;
	~Node() = default;

	static NodeHandler GetFreeNodeWithInit(const String& objectName)
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();
		auto ctx = pNodePool->GetContext();

		// 未使用ノードあり？
		if (ctx->m_stackFreeNodeID.Size() == 0)
		{
			return{};
		}

		NodeID id = ctx->m_stackFreeNodeID.Back();
		ctx->m_stackFreeNodeID.PopBack();

		Node<T>& node = ctx->m_vNodeSlot[id];
		
		// ノードのIDを設定
		NodeHandler handler;
		handler.id = id;
		handler.serial = ctx->m_nextSerial;

		node.SetHandler(handler);
		node.m_attach.SetNodeHandler(handler);
		// ノードの名前を設定
		node.SetName(objectName);
		// rootノードではない？
		if (id != 0)
		{
			Node<T>& rootNode = ctx->m_vNodeSlot[0];
			// rootノードを親に設定
			node.SetParentHandler(rootNode.GetHandler());
		}

		// 使用中のオブジェクト数を加算
		ctx->m_numOfObjects++;

		// シリアルをインクリメント
		ctx->m_nextSerial++;
		if (ctx->m_nextSerial == 0)
		{
			ctx->m_nextSerial = 1;
		}

		Logger::OutputDebug("spawn id %d, numOfObjects %d", id, ctx->m_numOfObjects);

		return handler;
	}

	T& GetAttach()
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
			pNodePool->m_vGcQueue.push_back(m_hNode.id);
		}
	}

	bool IsDestroyed() const
	{
		return m_destroyed;
	}

	NodeHandler GetHandler() const
	{
		return m_hNode;
	}

	void SetName(const String & name)
	{
		if (name.empty())
		{
			m_name.Reset();
			return;
		}

		m_name = Singleton<EqHeap>::GetInstance()->New<char>(name.size() + 1);
		if (m_name)
		{
			memcpy(m_name.Ref(), name.c_str(), name.size());
		}
	}

	String GetName()
	{
		if (!m_name)
		{
			return "";
		}

		return String(reinterpret_cast<char*>(m_name.Ref()));
	}

	void SetParentHandler(NodeHandler newParentHandler)
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();
		auto ctx = pNodePool->GetContext();

		if (m_hNode.id == 0)
		{
			Logger::OutputError(u8"rootノードに親を設定しようとしました。");
			return;
		}

		if (newParentHandler.id < 0)
		{
			// rootノードを取得
			auto rootNode = &(ctx->m_vNodeSlot[0]);
			newParentHandler = rootNode->GetHandler();
		}

		if (auto newParentNode = GetNodeByHandler(newParentHandler))
		{
			if (!newParentNode->IsDestroyed())
			{
				// 前の親の子リストから自分を削除
				DetachParent();

				// 新しい親の子リストに追加
				newParentNode->m_vChildren.PushBack(m_hNode);

				// 新しい親をセット
				m_hParent = newParentHandler;

				Logger::OutputDebug("obj %d, new parent %d", m_hNode.id, newParentNode->GetHandler().id);
			}
		}
	}

	NodeHandler GetParentHandler() const
	{
		if (auto parentNode = GetNodeByHandler(m_hParent))
		{
			if (!parentNode->IsDestroyed())
			{
				return m_hParent;
			}
		}

		return{};
	}

	void DetachChildren()
	{
		// 子を全て解除
		Logger::OutputDebug("obj %d, num children %d", m_hNode.id, m_vChildren.Size());

		for (auto& nodeHandler : m_vChildren)
		{
			if (auto node = GetNodeByHandler(nodeHandler))
			{
				if (!node->IsDestroyed())
				{
					NodeHandler empty;
					node->SetParentHandler(empty);	// -1を設定すると自動的に親はrootノードに設定される

					Logger::OutputDebug("  nodeID %d, detach childID %d", m_hNode.id, node->GetHandler().id);
				}
			}
		}
	}

	const EqVector<NodeHandler>& GetChildrenHandler() const
	{
		return m_vChildren;
	}

	int32_t GetChildCount() const
	{
		return m_vChildren.Size();
	}

	bool HasParent() const
	{
		return m_hParent.id >= 0;
	}

	static void GC()
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();
		auto ctx = pNodePool->GetContext();

		// 削除対象リストを元にオブジェクトの削除を行う
		for (auto& nodeID : pNodePool->m_vGcQueue)
		{
			ctx->m_vNodeSlot[nodeID].~Node<T>();
			new (&ctx->m_vNodeSlot[nodeID]) Node<T>();

			ctx->m_numOfObjects--;

			ctx->m_stackFreeNodeID.PushBack(nodeID);
		}

		// 削除対象リストクリア
		pNodePool->m_vGcQueue.clear();
	}

	static void DestroyThemAll()
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();
		auto ctx = pNodePool->GetContext();

		for (auto& node : ctx->m_vNodeSlot)
		{
			if (node.m_hNode.id >= 0)
			{
				node.Destroy();
			}
		}

		GC();

		Logger::OutputDebug("obj num %d, destroy %d, free %d", ctx->m_numOfObjects, pNodePool->m_vGcQueue.size(), ctx->m_stackFreeNodeID.Size());
	}

	static int32_t NumOfObjects()
	{
		auto ctx = Singleton<NodePool<T>>::GetInstance()->GetContext();
		return ctx->m_numOfObjects;
	}

	static Node<T>* GetNodeByHandler(const NodeHandler& handler)
	{
		auto ctx = Singleton<NodePool<T>>::GetInstance()->GetContext();

		if ( handler.id >= Singleton<NodePool<T>>::GetInstance()->defaultNum)
		{
			Logger::OutputError(u8"範囲外のノードが指定されました。 node id [%d]", handler.id);
			return nullptr;
		}

		if (handler.id >= 0)
		{
			auto node = &(ctx->m_vNodeSlot[handler.id]);
			if (node->GetHandler().serial == handler.serial)
			{
				return node;
			}
		}

		return nullptr;
	}

	static Node<T>* Root()
	{
		auto ctx = Singleton<NodePool<T>>::GetInstance()->GetContext();

		if (ctx->m_numOfObjects > 0)
		{
			return &(ctx->m_vNodeSlot[0]);
		}

		return nullptr;
	}

	// cbでfalseを返すとvisitを終了する
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

				for (auto& childHandler : beginNode.GetChildrenHandler())
				{
					Visit(ctx->m_vNodeSlot[childHandler.id], cb, nestDepth + 1);
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

		Logger::OutputInfo("vGcQueue %d, queFreeNode %d", pNodePool->m_vGcQueue.size(), pNodePool->m_stackFreeNodeID.size());
		Logger::OutputInfo("--- dump end ---");
#endif
	}

protected:
	void SetHandler(const NodeHandler& handler)
	{
		m_hNode = handler;
	}

	Node<T>* Self()
	{
		return GetNodeByHandler(m_hNode);
	}

private:

	// --- serialize begin ---
	NodeHandler m_hNode;				/// インスタンスのID、管理配列のインデックスと同じである
	NodeHandler m_hParent;				/// 親ノードのID、管理配列のインデックスと同じである
	EqVector<NodeHandler> m_vChildren;	/// 子ノードIDリスト
	EqHeap::Container<char> m_name;		/// ノードの名前
	bool m_destroyed = false;			/// 破棄フラグ trueにするとGC対象となる
	T m_attach;							/// アタッチされたオブジェクト
	// --- serialize end ---

	void DetachParent()
	{
		// 親のインスタンスが存在する？
		if (Node<T>* parent = GetNodeByHandler(m_hParent))
		{
			// 破棄されていない？
			if (!parent->IsDestroyed())
			{
				// 親の子リストから自分を削除する
				auto& refList = parent->m_vChildren;

				auto it = refList.begin();
				while (it != refList.end())
				{
					if (*it == m_hNode)
					{
						refList.Erase(it);
						break;
					}
					++it;
				}

				m_hParent = {};
			}
		}
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
		uint32_t m_nextSerial = 1;			// ノードに設定する通し番号
		EqVector<NodeID> m_stackFreeNodeID;		// 未使用状態のノードのIDを格納したスタック
	}Context;
	
	EqHeap::Container<Context> CreateContext()
	{
		auto handle = Singleton<EqHeap>::GetInstance()->New<Context>();
		if (handle)
		{
			if (auto ctx = handle.Ref())
			{
				ctx->m_stackFreeNodeID.Resize(defaultNum);

				int32_t index = defaultNum - 1;
				for (auto& elem : ctx->m_stackFreeNodeID)
				{
					elem = index;
					index--;
				}
			}
		}

		return handle;
	}

	void SetContext(EqHeap::Container<Context>& ctx)
	{
		m_pCtx = ctx.Ref();
	}

	Context* GetContext() { return m_pCtx; }

protected:

	friend Node<T>;
	Context* m_pCtx;

	std::vector<NodeID> m_vGcQueue;							// GC対象となるノードのIDを格納した配列

	NodePool() = default;
};

#endif
