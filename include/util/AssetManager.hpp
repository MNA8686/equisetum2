#if !defined(_EQASSETMANAGER_H_)
#define _EQASSETMANAGER_H_

#include "util/SharedPool.hpp"
#include "system/Singleton.h"
#include "graphic/Image.hpp"
#include "graphic/Sprite.hpp"
#include "graphic/FontManager.hpp"
#include "graphic/BitmapFont.hpp"
#include "audio/BGM.hpp"
#include "audio/SE.hpp"

namespace Equisetum2
{
	class Sprite;
	class AssetManager final
	{
	public:
		friend Singleton<AssetManager>;	// シングルトンからインスタンスを作成してもらえるようにする

		template<typename T>
		std::shared_ptr<T> Load(const String& id, bool shared=true)
		{
			EQ_DURING
			{
				if (shared)
				{
					// すでにロード済みのリソースなら共有ポインタを返す
					if (auto fromPool = Singleton<SharedPool<T>>::GetInstance()->GetByIdentify(id))
					{
						return fromPool;
					}
				}

				// 新規にロード
				auto res = _Load<T>(id);
				if (!res)
				{
					EQ_THROW(String::Sprintf(u8"リソース(%s)のロードに失敗しました。", id.c_str()));
				}

				if (!Singleton<SharedPool<T>>::GetInstance()->Register(id, res, shared))
				{
					EQ_THROW(String::Sprintf(u8"リソース(%s)の登録に失敗しました。", id.c_str()));
				}

				return res;
			}
			EQ_HANDLER
			{
				Logger::OutputError(EQ_GET_HANDLER().what());
			}
			EQ_END_HANDLER

			return nullptr;
		}

	private:

		AssetManager();
		virtual ~AssetManager() = default;

		template<typename T>
		static std::shared_ptr<T> _Load(const String& id)
		{
			return nullptr;
		}

		template<>
		static std::shared_ptr<Image> _Load(const String& id)
		{
			return _LoadImage(id);
		}

		/*
		const char* json = R"({
			"type": "sprite",
			"name": "subaru",
			"textureid": "jiki_subaru",
			"atlas": [
				{
					"pos" : [0, 0],
					"size" : [80, 80],
					"pivot" : [0.5, 0.5]
				},
				{
					"matrix": [6, 2],
					"pos" : [0, 80],
					"size" : [80, 80],
					"pivot" : [0.5, 0.5]
				}
			]
		})";
		*/
		template<>
		static std::shared_ptr<Sprite> _Load(const String& id)
		{
			return _LoadSprite(id);
		}

		template<>
		static std::shared_ptr<BitmapFont> _Load(const String& id)
		{
			return _LoadBitmapFont(id);
		}

		template<>
		static std::shared_ptr<FontManager> _Load(const String& id)
		{
			return _LoadFontManager(id);
		}

		template<>
		static std::shared_ptr<SE> _Load(const String& id)
		{
			return _LoadSE(id);
		}

		template<>
		static std::shared_ptr<BGM> _Load(const String& id)
		{
			return _LoadBGM(id);
		}

		template<>
		static std::shared_ptr<Texture> _Load(const String& id)
		{
			return _LoadTexture(id);
		}

		static std::shared_ptr<Image> _LoadImage(const String& id);
		static std::shared_ptr<Sprite> _LoadSprite(const String& id);
		static std::shared_ptr<BitmapFont> _LoadBitmapFont(const String& id);
		static std::shared_ptr<FontManager> _LoadFontManager(const String& id);
		static std::shared_ptr<BGM> _LoadBGM(const String& id);
		static std::shared_ptr<SE> _LoadSE(const String& id);
		static std::shared_ptr<Texture> _LoadTexture(const String& id);
	};
}

#endif
