#if !defined(_EQNODE_H_)
#define _EQNODE_H_

#include "Equisetum2.h"
using namespace Equisetum2;

#include <queue>
#include <vector>
#include <list>
#include <functional>

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/list.hpp>

using NodeID = int32_t;

class Node
{
public:

	void Destroy();
	bool IsDestroyed() const;

	Node();
	virtual ~Node();

	NodeID GetID() const;
	void SetName(const String& name);
	String GetName();

	void SetParent(std::shared_ptr<Node> parent);
	std::shared_ptr<Node> GetParent() const;
	void DetachChildren();
	std::vector<std::shared_ptr<Node>> GetChildren() const;
	int32_t GetChildCount() const;
	bool HasParent() const;

	static std::shared_ptr<Node> Create(const String& objectName);
	static void GC();
	static void DestroyThemAll();
	static int32_t NumOfObjects();
	static void Dump();

	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(CEREAL_NVP(m_nodeID));
		archive(CEREAL_NVP(m_parentId));
		archive(CEREAL_NVP(m_listChildren));
		archive(CEREAL_NVP(m_destroyed));

		std::string& m_name_str = m_name;
		archive(CEREAL_NVP(m_name_str));
	}

protected:
	static bool Init(std::shared_ptr<Node> pNode, const String& name);
	void SetID(NodeID id);
	std::list<NodeID>& GetChildrenID();
	std::shared_ptr<Node>& GetNodeByID(NodeID nodeID);
	NodeID GetParentID() const;

private:

	// --- serialize begin ---
	NodeID m_nodeID = -1;		/// インスタンスのID、管理配列のインデックスと同じである
	NodeID m_parentId = -1;		/// 親ノードのID、管理配列のインデックスと同じである
	std::list<NodeID> m_listChildren;		/// 子ノードIDリスト
	String m_name;		/// ノードの名前
	bool m_destroyed = false;	// 破棄フラグ trueにするとGC対象となる
	// --- serialize end ---

	// cbでfalseを返すとvisitを終了する
	static bool Visit(std::shared_ptr<Node> beginNode, const std::function<bool(std::shared_ptr<Node>&, int32_t nestDepth)>& cb, int32_t nestDepth=0);
	void DetachParent();
};

class NodePool final
{
public:
	friend Singleton<NodePool>;	// シングルトンからインスタンスを作成してもらえるようにする

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

	friend Node;

	// --- serialize begin ---
	std::vector<std::shared_ptr<Node>> m_vNodeSlot;		// ノードの実体を格納した配列
	std::queue<NodeID> m_queFreeNode;					// 未使用状態のノードのIDを格納したキュー
	int32_t m_numOfObjects = 0;							// 現在生成されているノードの数
	std::list<std::weak_ptr<Node>> m_listZombie;		// ゾンビノードリスト
	// --- serialize end ---

	std::vector<NodeID> m_vGcQueue;					// GC対象となるノードのIDを格納した配列

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
