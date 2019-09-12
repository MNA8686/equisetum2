#if !defined(_EQBITMAPFONT_H_)
#define _EQBITMAPFONT_H_

#include "type/String.h"
#include "graphic/Sprite.hpp"
#include <memory>

namespace Equisetum2
{
	class BitmapFont
	{
	public:
#if 0
		struct SerializeHint
		{
			enum class From : int32_t
			{
				FromFontManager,	// FontManagerで作られた
				FromAsset,			// アセットとしてロードされた
				//Raw,	// don't work
			};

			From from;
			String param;
		};
#endif

		BitmapFont() = default;
		virtual ~BitmapFont() = default;

		static std::shared_ptr<BitmapFont> CreateFromSprite(const std::shared_ptr<Sprite>& sprite, const String& codePoint);

		/**
		* @brief 新しいインスタンスで現在のインスタンスを置き換える
		* @param src 置き換え元のインスタンス
		* @return 成否
		*/
		virtual bool MoveFrom(std::shared_ptr<BitmapFont>&& src);

		/**
		* @brief インスタンスのIDを設定する
		*/
		virtual void SetIdentify(const String& id);

		/**
		* @brief インスタンスのIDを取得する
		* @return インスタンスのID
		*/
		virtual String Identify() const;

		const std::shared_ptr<Sprite>& GetSprite() const;
		int CodePointToAtlas(char32_t code);

	private:

		String m_id;		/// SpriteのID
		std::shared_ptr<Sprite> m_sprite;		/// 画像本体
		std::u32string m_codePoint;		/// コードポイント配列
	};
}

#endif
