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
	NodeID m_nodeID = -1;		/// �C���X�^���X��ID�A�Ǘ��z��̃C���f�b�N�X�Ɠ����ł���
	NodeID m_parentId = -1;		/// �e�m�[�h��ID�A�Ǘ��z��̃C���f�b�N�X�Ɠ����ł���
	std::list<NodeID> m_listChildren;		/// �q�m�[�hID���X�g
	String m_name;		/// �m�[�h�̖��O
	bool m_destroyed = false;	// �j���t���O true�ɂ����GC�ΏۂƂȂ�
	// --- serialize end ---

	// cb��false��Ԃ���visit���I������
	static bool Visit(std::shared_ptr<Node> beginNode, const std::function<bool(std::shared_ptr<Node>&, int32_t nestDepth)>& cb, int32_t nestDepth=0);
	void DetachParent();
};

class NodePool final
{
public:
	friend Singleton<NodePool>;	// �V���O���g������C���X�^���X���쐬���Ă��炦��悤�ɂ���

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
	std::vector<std::shared_ptr<Node>> m_vNodeSlot;		// �m�[�h�̎��̂��i�[�����z��
	std::queue<NodeID> m_queFreeNode;					// ���g�p��Ԃ̃m�[�h��ID���i�[�����L���[
	int32_t m_numOfObjects = 0;							// ���ݐ�������Ă���m�[�h�̐�
	std::list<std::weak_ptr<Node>> m_listZombie;		// �]���r�m�[�h���X�g
	// --- serialize end ---

	std::vector<NodeID> m_vGcQueue;					// GC�ΏۂƂȂ�m�[�h��ID���i�[�����z��

	NodePool()	// �C���X�^���X�쐬����
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
