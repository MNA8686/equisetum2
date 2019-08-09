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

		// ���g�p�m�[�h����H
		if (ctx->m_queFreeNode.Size() == 0)
		{
			return -1;
		}

		NodeID id = ctx->m_queFreeNode.Back();
		ctx->m_queFreeNode.PopBack();

		Node<T>& node = ctx->m_vNodeSlot[id];
		
		// �m�[�h��ID��ݒ�
		node.SetID(id);
		node.m_attach.SetNodeID(id);
		// �m�[�h�̖��O��ݒ�
		node.SetName(objectName);
		// root�m�[�h�ł͂Ȃ��H
		if (id != 0)
		{
			// root�m�[�h��e�ɐݒ�
			node.SetParent(0);
		}

		// �g�p���̃I�u�W�F�N�g�������Z
		ctx->m_numOfObjects++;

		// �č\�z�t���O�Z�b�g
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

	//void SetParent(std::shared_ptr<Node<T>> newParent)
	void SetParent(NodeID newParent)
	{
		auto pNodePool = Singleton<NodePool<T>>::GetInstance();
		auto ctx = pNodePool->GetContext();

		if (m_nodeID == 0)
		{
			Logger::OutputError(u8"root�m�[�h�ɐe��ݒ肵�悤�Ƃ��܂����B");
			return;
		}

		//if (!newParent)
		if (newParent < 0)
		{
			// root�m�[�h���擾
			newParent = 0;// pContext->m_vNodeSlot[0].GetID();
		}

		auto& newParentNode = ctx->m_vNodeSlot[newParent];
		//if (!newParent->IsDestroyed())
		if (!newParentNode.IsDestroyed())
		{
			// �O�̐e�̎q���X�g���玩�����폜
			DetachParent();

			// �V�����e�̎q���X�g�ɒǉ�
			newParentNode.m_listChildren.PushBack(m_nodeID);

			// �V�����e���Z�b�g
			m_parentId = newParent;// newParent->GetID();

			//Logger::OutputDebug("obj %d, new parent %d", m_nodeID, newParent->GetID());
			Logger::OutputDebug("obj %d, new parent %d", m_nodeID, newParentNode.GetID());

			// �č\�z�t���O�Z�b�g
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

		// �q��S�ĉ���
		Logger::OutputDebug("obj %d, num children %d", m_nodeID, m_listChildren.Size());

		for (auto& nodeID : m_listChildren)
		{
			auto& node = ctx->m_vNodeSlot[nodeID];

			if(!node.IsDestroyed())
			{
				node.SetParent(-1);	// nullptr��ݒ肷��Ǝ����I�ɐe��root�m�[�h�ɐݒ肳���

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
			// �X�P�W���[���z����폜
			pNodePool->m_vNodeScheduler.clear();
		}
#endif

		// �폜�Ώۃ��X�g�����ɃI�u�W�F�N�g�̍폜���s��
		for (auto& nodeID : pNodePool->m_vGcQueue)
		{
		//	pNodePool->m_listZombie.push_back(pNodePool->m_vNodeSlot[nodeID]);	// �]���r���o�p

			ctx->m_vNodeSlot[nodeID] = {};		//TODO:��������́H
			ctx->m_numOfObjects--;

			ctx->m_queFreeNode.PushBack(nodeID);
		}

		// �폜�Ώۃ��X�g�N���A
		pNodePool->m_vGcQueue.clear();

#if 0
		// �]���r���o���s��
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
			// �A�T�[�g
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

	// cb��false��Ԃ���visit���I������
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
	NodeID m_nodeID = -1;				/// �C���X�^���X��ID�A�Ǘ��z��̃C���f�b�N�X�Ɠ����ł���
	NodeID m_parentId = -1;				/// �e�m�[�h��ID�A�Ǘ��z��̃C���f�b�N�X�Ɠ����ł���
	EqVector<NodeID> m_listChildren;	/// �q�m�[�hID���X�g
	String m_name;						/// �m�[�h�̖��O
	bool m_destroyed = false;			/// �j���t���O true�ɂ����GC�ΏۂƂȂ�
	T m_attach;							/// �A�^�b�`���ꂽ�I�u�W�F�N�g
	// --- serialize end ---
#if 0
	// --- serialize begin ---
	NodeID m_nodeID = -1;				/// �C���X�^���X��ID�A�Ǘ��z��̃C���f�b�N�X�Ɠ����ł���
	NodeID m_parentId = -1;				/// �e�m�[�h��ID�A�Ǘ��z��̃C���f�b�N�X�Ɠ����ł���
	std::list<NodeID> m_listChildren;	/// �q�m�[�hID���X�g
	String m_name;						/// �m�[�h�̖��O
	bool m_destroyed = false;			/// �j���t���O true�ɂ����GC�ΏۂƂȂ�
	std::shared_ptr<T> m_attach;		/// �A�^�b�`���ꂽ�I�u�W�F�N�g
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

		// �e�̃C���X�^���X�����݂���H
		Node<T>& parent = ctx->m_vNodeSlot[m_parentId];

		// �j������Ă��Ȃ��H
		if (!parent.IsDestroyed())
		{
			// �e�̎q���X�g���玩�����폜����
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

			// �č\�z�t���O�Z�b�g
//			pNodePool->m_dirty = true;
		}
#endif
	}
};

template<class T>
class NodePool final
{
public:
	friend Singleton<NodePool<T>>;	// �V���O���g������C���X�^���X���쐬���Ă��炦��悤�ɂ���

	static const int32_t defaultNum = 4096;

	typedef struct
	{
		Node<T> m_vNodeSlot[defaultNum];	// �m�[�h�̎��̂��i�[�����z��
		int32_t m_numOfObjects = 0;			// ���ݐ�������Ă���m�[�h�̐�
		EqVector<NodeID> m_queFreeNode;		// ���g�p��Ԃ̃m�[�h��ID���i�[�����L���[
		//EqVector<NodeID> m_listZombie;		// �]���r�m�[�h���X�g
		Node<T> m_null;	// �k���m�[�h
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
	std::vector<std::shared_ptr<Node<T>>> m_vNodeSlot;		// �m�[�h�̎��̂��i�[�����z��
	std::queue<NodeID> m_queFreeNode;					// ���g�p��Ԃ̃m�[�h��ID���i�[�����L���[
	int32_t m_numOfObjects = 0;							// ���ݐ�������Ă���m�[�h�̐�
	std::list<std::weak_ptr<Node<T>>> m_listZombie;		// �]���r�m�[�h���X�g
	// --- serialize end ---
#endif

	Context* m_pCtx;

	std::vector<NodeID> m_vGcQueue;							// GC�ΏۂƂȂ�m�[�h��ID���i�[�����z��
//	bool m_dirty = true;									// �X�P�W���[���z��̍č\�z���K�v���ǂ���
//	std::vector<std::shared_ptr<Node<T>>> m_vNodeScheduler;	// �X�P�W���[���z��

	NodePool() = default;
	//{
	//	Reset();
	//}
};

#endif
