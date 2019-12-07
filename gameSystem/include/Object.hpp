#if !defined(_EQOBJECT_H_)
#define _EQOBJECT_H_

#include "Equisetum2.h"
using namespace Equisetum2;

#include "INodeAttachment.hpp"
#include "Script.hpp"
#include "EqVector.hpp"
#include "ScriptContainer.hpp"

struct stAsset;

class Object final : public INodeAttachment 
{
public:

	Object();
	~Object();

	static NodeHandler Create(const String& id, NodeHandler parent = {});
	NodeHandler CreateChild(const String & id);

	const Point_t<FixedDec>& GetPos() const;
	const Point_t<FixedDec>& GetLocalPos() const;
	void SetPos(const Point_t<FixedDec>& pos);
	void MovePos(const Point_t<FixedDec>& vec);
	void SetLocalPos(const Point_t<FixedDec>& pos);
	void MoveLocalPos(const Point_t<FixedDec>& vec);
	bool GetRelativeParent() const;
	void SetRelativeParent(bool on);
	int32_t AddRenderObject(std::shared_ptr<RenderObject> renderObject);
	RenderObject* GetRenderObject(int32_t index);
	bool OnDraw(std::shared_ptr<Renderer>& renderer);
	stAsset* GetAsset();
	bool OnBeforeUpdate();
	bool OnFixedUpdate();
	bool OnLateUpdate();

	static Object* GetRoot();
	static Object* GetObjectByHandler(const NodeHandler& handler);
	static Object* GetObjectByName(Object* beginNode, const String& name);
	Object* Self();
	bool HasParent() const;
	NodeHandler GetParentHandler() const;

	void Destroy(bool late=false);
	bool IsDestroyed() const;
	void SetParentHandler(const NodeHandler& newParentHandler);
	Object* GetParent() const;
	void DetachChildren();
	int32_t GetChildCount() const;
	const EqVector<NodeHandler>& GetChildrenHandler() const;

	std::shared_ptr<Object> Fork();

	bool IsActive() const;
	bool IsVisible() const;
	bool IsScheduled() const;

	void SetActive(bool active);
	void SetVisible(bool visible);
	void SetSchedule(bool schedule);

	// �X�P�W���[�����s
	static void Reschedule();
	static void Update();
	static Object* GetCreatingObject();

	template<class T>
	const T* GetScript() const
	{
		if (auto script = m_script.Ref())
		{
			return script->Ref<T>();
		}

		return nullptr;
	}

	template<class T>
	T* GetScript()
	{
		if (auto script = m_script.Ref())
		{
			return script->Ref<T>();
		}

		return nullptr;
	}

protected:

private:

	// --- serialize begin ---
	Point_t<FixedDec> m_pos;		/// ���[���h���W
	Point_t<FixedDec> m_localPos;	/// �e�Ƃ̑��΍��W�B�e�����Ȃ����A�܂��͐e�ɒǏ]���Ȃ�����m_pos�Ɠ����B
	EqHeap::Container<ScriptContainer> m_script;	/// �X�N���v�g
	bool m_relativeParent = true;	/// �e�̍��W�ɒǏ]���邩�ǂ���
	bool m_active = true;			/// false�̏ꍇ�A�X�N���v�g�Ȃǂ��Ăяo����Ȃ�
	bool m_visible = true;			/// false�̏ꍇ�A�����_�����O�ΏۂƂȂ�Ȃ�
	bool m_isScheduled = true;		/// �X�P�W���[�����邩�ǂ���(Visit�����ł͌Ăяo���K�v���Ȃ��ꍇ��false�ɂ���B)
	// --- serialize end ---

	/// �q�ɐe�̍��W�ړ��𔽉f������
	void SetPosForChild();

	static bool m_dirty;		/// �X�P�W���[���z��č\�z�t���O
	static std::vector<Object*> m_creatingObj;		/// ���ݍ쐬���̃I�u�W�F�N�g
};

struct ObjectContainer : NodeHandler
{
	operator bool() const;
	operator NodeHandler() const;
	const Object* Ref() const;
	const Object* operator ->() const;
	Object* Ref();
	Object* operator ->();
	ObjectContainer& operator =(const NodeHandler& handler);
};

#endif
