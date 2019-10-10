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

		struct SerializeHint
		{
			enum class From : int32_t
			{
				Empty,
				FontManager,	// FontManagerで作られた
				Asset,			// アセットとしてロードされた
				//Raw,	// don't work
			};

			From from = From::Empty;
			String id;
			Color color{};
			Size maxSize;
		};

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

		/**
		* @brief コードポイント文字列を取得する
		* @return コードポイント文字列
		*/
		virtual String CodePoint() const;

		/**
		* @brief 設定されているスプライトを取得する
		* @return 設定されているスプライト
		*/
		const std::shared_ptr<Sprite>& GetSprite() const;

		/**
		* @brief コードポイントがスプライトアトラスの何番目に割り当てられているかを返す
		* @return アトラス番号
		*/
		int CodePointToAtlas(char32_t code);

		/**
		* @brief シリアライズに必要なヒントを設定する
		* @param hint 設定するヒント
		* @return なし 
		*/
		virtual void SetHint(const SerializeHint& hint);

		/**
		* @brief シリアライズに必要なヒントを取得する
		* @return ヒント
		*/
		virtual SerializeHint GetHint(void) const;

	private:

		String m_id;		/// SpriteのID
		std::shared_ptr<Sprite> m_sprite;		/// 画像本体
		std::u32string m_codePoint;		/// コードポイント配列
		SerializeHint m_hint;			/// シリアライズ用ヒント
	};
}

#endif
