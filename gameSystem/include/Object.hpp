#if !defined(_EQOBJECT_H_)
#define _EQOBJECT_H_

#include "Equisetum2.h"
using namespace Equisetum2;

#include "INodeAttachment.hpp"
#include "Script.hpp"
#include "EqVector.hpp"

struct stAsset;

class Object final : public INodeAttachment 
{
public:

	Object();
	~Object();

	static NodeHandler Create(const String& id);
	//NodeID CreateChild(const String & id);

	const Point_t<FixedDec>& GetPos() const;
	const Point_t<FixedDec>& GetLocalPos() const;
	void SetPos(const Point_t<FixedDec>& pos);
	void SetLocalPos(const Point_t<FixedDec>& pos);
	bool GetRelativeParent() const;
	void SetRelativeParent(bool on);
	void AddRenderObject(std::shared_ptr<RenderObject> renderObject);
	bool OnDraw(std::shared_ptr<Renderer>& renderer);
	stAsset* GetAsset();
	bool OnFixedUpdate();

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

	// スケジュール実行
	static void Update();

protected:

private:

	// --- serialize begin ---
	std::vector<std::shared_ptr<RenderObject>> m_vRenderObject;	/// レンダーオブジェクト配列
	Point_t<FixedDec> m_pos;		/// ワールド座標
	Point_t<FixedDec> m_localPos;	/// 親との相対座標。親がいない時、または親に追従しない時はm_posと同じ。
	bool m_relativeParent = true;	/// 親の座標に追従するかどうか
//	int32_t m_angle = 0;
	bool m_active = true;			/// falseの場合、スクリプトなどが呼び出されない
	bool m_visible = true;			/// falseの場合、レンダリング対象とならない
	// --- serialize end ---

	/// 子に親の座標移動を反映させる
	void SetPosForChild();

	static std::vector<NodeHandler> m_vUpdate;		/// スケジュール配列

public:
	static bool m_dirty;		/// スケジュール配列再構築フラグ
};

#endif
