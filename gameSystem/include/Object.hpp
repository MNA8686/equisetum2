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

	// スケジュール実行
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
	Point_t<FixedDec> m_pos;		/// ワールド座標
	Point_t<FixedDec> m_localPos;	/// 親との相対座標。親がいない時、または親に追従しない時はm_posと同じ。
	EqHeap::Container<ScriptContainer> m_script;	/// スクリプト
	bool m_relativeParent = true;	/// 親の座標に追従するかどうか
	bool m_active = true;			/// falseの場合、スクリプトなどが呼び出されない
	bool m_visible = true;			/// falseの場合、レンダリング対象とならない
	bool m_isScheduled = true;		/// スケジュールするかどうか(Visit処理では呼び出す必要がない場合にfalseにする。)
	// --- serialize end ---

	/// 子に親の座標移動を反映させる
	void SetPosForChild();

	static bool m_dirty;		/// スケジュール配列再構築フラグ
	static std::vector<Object*> m_creatingObj;		/// 現在作成中のオブジェクト
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
