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
			// �폜�t���O��ݒ�
			m_destroyed = true;

			// �e�̎Q�ƃ��X�g���玩�����폜
			DetachParent();

			// �S�Ă̎q���f�^�b�`
			DetachChildren();

			// �폜�L���[�ɓ����(���GC�ł��̃I�u�W�F�N�g�͔j�������)
			pNodePool->m_vGcQueue.push_back(m_nodeID);

			// �č\�z�t���O�Z�b�g
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

			// �č\�z�t���O�Z�b�g
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
			// �X�P�W���[���z����폜
			pNodePool->m_vNodeScheduler.clear();
		}
#endif

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
			// �A�T�[�g
		}

		return pNodePool->m_vNodeSlot[nodeID];
	}

	std::list<NodeID>& GetChildrenID()
	{
		return m_listChildren;
	}

	// cb��false��Ԃ���visit���I������
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
			// �X�P�W���[���z����N���A
			pNodePool->m_vNodeScheduler.clear();

			// ���[�g�m�[�h�擾
			if (auto pThisNode = pNodePool->m_vNodeSlot[0])
			{
				// �m�[�h��H��A�X�P�W���[���z��ɒǉ����Ă���
				Visit(pThisNode, [](std::shared_ptr<Node<T>>& node, int32_t nestDepth)->bool {
					bool add = node->AddScheduler();		// �ǉ���������
					if (add)
					{
						// �X�P�W���[���z��Ƀm�[�h��ǉ�
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

		// �č\�z�t���O�Z�b�g
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
	NodeID m_nodeID = -1;				/// �C���X�^���X��ID�A�Ǘ��z��̃C���f�b�N�X�Ɠ����ł���
	NodeID m_parentId = -1;				/// �e�m�[�h��ID�A�Ǘ��z��̃C���f�b�N�X�Ɠ����ł���
	std::list<NodeID> m_listChildren;	/// �q�m�[�hID���X�g
	String m_name;						/// �m�[�h�̖��O
	bool m_destroyed = false;			/// �j���t���O true�ɂ����GC�ΏۂƂȂ�
	std::shared_ptr<T> m_attach;		/// �A�^�b�`���ꂽ�I�u�W�F�N�g
	// --- serialize end ---

	void DetachParent()
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();

		if (m_parentId < 0)
		{
			return;
		}

		// �e�̃C���X�^���X�����݂���H
		if (std::shared_ptr<Node<T>> pParent = pNodePool->m_vNodeSlot[m_parentId])
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

			// �č\�z�t���O�Z�b�g
//			pNodePool->m_dirty = true;
		}
	}
};

template<class T>
class NodePool final
{
public:
	friend Singleton<NodePool<T>>;	// �V���O���g������C���X�^���X���쐬���Ă��炦��悤�ɂ���

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
	std::vector<std::shared_ptr<Node<T>>> m_vNodeSlot;		// �m�[�h�̎��̂��i�[�����z��
	std::queue<NodeID> m_queFreeNode;					// ���g�p��Ԃ̃m�[�h��ID���i�[�����L���[
	int32_t m_numOfObjects = 0;							// ���ݐ�������Ă���m�[�h�̐�
	std::list<std::weak_ptr<Node<T>>> m_listZombie;		// �]���r�m�[�h���X�g
	// --- serialize end ---

	std::vector<NodeID> m_vGcQueue;							// GC�ΏۂƂȂ�m�[�h��ID���i�[�����z��
//	bool m_dirty = true;									// �X�P�W���[���z��̍č\�z���K�v���ǂ���
//	std::vector<std::shared_ptr<Node<T>>> m_vNodeScheduler;	// �X�P�W���[���z��

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
