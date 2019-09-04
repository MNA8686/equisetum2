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
	void SetLocalPos(const Point_t<FixedDec>& pos);
	bool GetRelativeParent() const;
	void SetRelativeParent(bool on);
	int32_t AddRenderObject(std::shared_ptr<RenderObject> renderObject);
	RenderObject* GetRenderObject(int32_t index);
	bool OnDraw(std::shared_ptr<Renderer>& renderer);
	stAsset* GetAsset();
	bool OnFixedUpdate();

	static Object* GetRoot();
	static Object* GetObjectByHandler(const NodeHandler& handler);
	Object* Self();
	bool HasParent() const;
	NodeHandler GetParentHandler() const;

	void Destroy();
	bool IsDestroyed() const;
	void SetParentHandler(const NodeHandler& newParentHandler);
	Object* GetParent() const;
	void DetachChildren();
	int32_t GetChildCount() const;
	const EqVector<NodeHandler>& GetChildrenHandler() const;

	std::shared_ptr<Object> Fork();

	bool IsActive() const;
	bool IsVisible() const;

	void SetActive(bool active);
	void SetVisible(bool visible);

	// �X�P�W���[�����s
	static void Update();

protected:

private:

	// --- serialize begin ---
	Point_t<FixedDec> m_pos;		/// ���[���h���W
	Point_t<FixedDec> m_localPos;	/// �e�Ƃ̑��΍��W�B�e�����Ȃ����A�܂��͐e�ɒǏ]���Ȃ�����m_pos�Ɠ����B
	EqHeap::Container<ScriptContainer> m_script;	/// �X�N���v�g
	bool m_relativeParent = true;	/// �e�̍��W�ɒǏ]���邩�ǂ���
	bool m_active = true;			/// false�̏ꍇ�A�X�N���v�g�Ȃǂ��Ăяo����Ȃ�
	bool m_visible = true;			/// false�̏ꍇ�A�����_�����O�ΏۂƂȂ�Ȃ�
	// --- serialize end ---

	/// �q�ɐe�̍��W�ړ��𔽉f������
	void SetPosForChild();

public:
	static bool m_dirty;		/// �X�P�W���[���z��č\�z�t���O
};

#endif
