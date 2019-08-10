#if !defined(_EQOBJECT_H_)
#define _EQOBJECT_H_

#include "Equisetum2.h"
using namespace Equisetum2;

#include "INodeAttachment.hpp"
#include "Script.hpp"
#include "EqVector.hpp"

/**
  アセット管理構造体
*/
class Script;
struct stAsset
{
	std::vector<std::shared_ptr<Sprite>> m_sprite;
	std::vector<std::shared_ptr<BGM>> m_bgm;
	std::vector<std::shared_ptr<SE>> m_se;
	std::vector<std::shared_ptr<Script>> m_script;

#if 0
	// アセットはIDのみを保存しておき、リストア時にはロードし直す
	// (スクリプトは除く)
	template<class Archive>
	void save(Archive & archive) const
	{
		// スプライト
		{
			std::vector<std::string> spriteID;
			for (auto& sprite : m_sprite)
			{
				spriteID.push_back(sprite ? sprite->Identify() : "");
			}
			archive(CEREAL_NVP(spriteID));
		}

		// BGM
		{
			std::vector<std::string> bgmID;
			for (auto& p : m_bgm)
			{
				bgmID.push_back(p ? p->Identify() : "");
			}
			archive(CEREAL_NVP(bgmID));
		}

		// SE
		{
			std::vector<std::string> seID;
			for (auto& p : m_se)
			{
				seID.push_back(p ? p->Identify() : "");
			}
			archive(CEREAL_NVP(seID));
		}

		// スクリプト
		{
			archive(CEREAL_NVP(m_script));
		}
	}

	template<class Archive>
	void load(Archive & archive)
	{
		// スプライト
		{
			std::vector<std::string> spriteID;
			archive(CEREAL_NVP(spriteID));

			int index = 0;
			m_sprite.clear();
			for (auto& id : spriteID)
			{
				m_sprite.push_back(id.empty() ? nullptr : Singleton<AssetManager>::GetInstance()->Load<Sprite>(id));
				index++;
			}
		}

		// BGM
		{
			std::vector<std::string> bgmID;
			archive(CEREAL_NVP(bgmID));

			int index = 0;
			m_bgm.clear();
			for (auto& id : bgmID)
			{
				m_bgm.push_back(id.empty() ? nullptr : Singleton<AssetManager>::GetInstance()->Load<BGM>(id));
				index++;
			}
		}

		// SE
		{
			std::vector<std::string> seID;
			archive(CEREAL_NVP(seID));

			int index = 0;
			m_se.clear();
			for (auto& id : seID)
			{
				m_se.push_back(id.empty() ? nullptr : Singleton<AssetManager>::GetInstance()->Load<SE>(id));
				index++;
			}
		}

		// スクリプト
		{
			archive(CEREAL_NVP(m_script));
		}
	}
#endif
};

class Object final : public INodeAttachment 
{
public:

	Object();
	~Object();

	static NodeID Create(const String& id);
	NodeID CreateChild(const String & id);

	const Point_t<FixedDec>& GetPos() const;
	const Point_t<FixedDec>& GetLocalPos() const;
	void SetPos(const Point_t<FixedDec>& pos);
	void SetLocalPos(const Point_t<FixedDec>& pos);
	bool GetRelativeParent() const;
	void SetRelativeParent(bool on);
	void AddRenderObject(std::shared_ptr<RenderObject> renderObject);
	bool OnDraw(std::shared_ptr<Renderer>& renderer);
	stAsset& GetAsset();
	bool OnFixedUpdate();

	static Object& GetObjectByID(NodeID id);
	Object& Self();
	bool HasParent() const;
	NodeID GetParentID() const;

	void SetNodeID(NodeID id) override;
	NodeID GetNodeID() const override;

	void Destroy();
	bool IsDestroyed() const;
	void SetParentID(NodeID newParent);
	Object& GetParent() const;
	void DetachChildren();
	int32_t GetChildCount() const;
	const EqVector<NodeID>& GetChildrenID() const;

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
	stAsset m_asset;				/// アセット管理構造体
	std::vector<std::shared_ptr<RenderObject>> m_vRenderObject;	/// レンダーオブジェクト配列
	Point_t<FixedDec> m_pos;		/// ワールド座標
	Point_t<FixedDec> m_localPos;	/// 親との相対座標。親がいない時、または親に追従しない時はm_posと同じ。
	bool m_relativeParent = true;	/// 親の座標に追従するかどうか
//	int32_t m_angle = 0;
	bool m_active = true;			/// falseの場合、スクリプトなどが呼び出されない
	bool m_visible = true;			/// falseの場合、レンダリング対象とならない
	NodeID m_nodeID = -1;			/// アタッチしているノードのID
	// --- serialize end ---

	/// 子に親の座標移動を反映させる
	void SetPosForChild();

//	static bool m_dirty;		/// スケジュール配列再構築フラグ
	static std::vector<NodeID> m_vUpdate;		/// スケジュール配列

public:
	static bool m_dirty;		/// スケジュール配列再構築フラグ
};

#endif
