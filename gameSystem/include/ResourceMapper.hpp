#if !defined(_RESOURCEMAPPER_H_)
#define _RESOURCEMAPPER_H_

#include "Equisetum2.h"
using namespace Equisetum2;

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
//#include <cereal/types/list.hpp>

/**
アセット管理構造体
*/
class Script;
struct stAsset
{
	std::vector<std::shared_ptr<Sprite>> m_sprite;
	std::vector<std::shared_ptr<BGM>> m_bgm;
	std::vector<std::shared_ptr<SE>> m_se;
	std::vector<std::shared_ptr<FontManager>> m_font;

	// アセットはIDのみを保存しておき、リストア時にはロードし直す
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
		
		// フォントマネージャー
		{
			std::vector<std::string> fontManagerID;
			for (auto& p : m_font)
			{
				fontManagerID.push_back(p ? p->Identify() : "");
			}
			archive(CEREAL_NVP(fontManagerID));
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

		// フォントマネージャー
		{
			std::vector<std::string> fontManagerID;
			archive(CEREAL_NVP(fontManagerID));

			int index = 0;
			m_font.clear();
			for (auto& id : fontManagerID)
			{
				m_font.push_back(id.empty() ? nullptr : Singleton<FontManager>::GetInstance()->Load<FontManager>(id));
				index++;
			}
		}
	}
};

struct stMappedResource
{
	stAsset asset;
	std::vector<std::shared_ptr<RenderObject>> renderObject;	/// レンダーオブジェクト配列

	template<class Archive>
	void save(Archive & archive) const
	{
		archive(CEREAL_NVP(asset));
		archive(CEREAL_NVP(renderObject));
	}

	template<class Archive>
	void load(Archive & archive)
	{
		archive(CEREAL_NVP(asset));
		archive(CEREAL_NVP(renderObject));
	}
};

class ResourceMapper
{
public:
	friend Singleton<ResourceMapper>;	// シングルトンからインスタンスを作成してもらえるようにする

	stMappedResource* Map(int32_t id);
	void Unmap(int32_t id);
	void Reset();

	template<class Archive>
	void save(Archive & archive) const
	{
		archive(CEREAL_NVP(m_vMapped));
	}

	template<class Archive>
	void load(Archive & archive)
	{
		archive(CEREAL_NVP(m_vMapped));
	}

private:
	std::vector<stMappedResource> m_vMapped;
};

#endif
