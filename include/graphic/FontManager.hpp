#if !defined(_EQFONTMANAGER_H_)
#define _EQFONTMANAGER_H_

#include "stream/IStream.h"
#include "graphic/BitmapFont.hpp"
#include <memory>
#include <map>

#include <cereal/cereal.hpp>

namespace Equisetum2
{
	typedef struct
	{
		String u8str;		// コードポイント
		Size size;			// レンダリング時のサイズ
	}stGlyphInfo;

	class FontManager
	{
	public:

		static std::shared_ptr<FontManager> CreateFromStream(std::shared_ptr<IStream> stream, int fontSize);

		std::shared_ptr<BitmapFont> MakeBitmapFont(const String& sampleStr, Color color, Size maxSize = { 1024, 1024 });
		Optional<std::map<char32_t, stGlyphInfo>> GetGlyphInfoBySampleString(const String& sampleStr);
		std::shared_ptr<Sprite> MakeSpriteByGlyphInfo(const std::map<char32_t, stGlyphInfo>& mapGlyphInfo, Color inColor, Size maxSize = { 1024, 1024 });

		/**
		* @brief イメージのIDを設定する
		*/
		virtual void SetIdentify(const String& id);

		/**
		* @brief イメージのIDを取得する
		* @return イメージのID
		*/
		virtual String Identify() const;

		int32_t GetFontSize() const;

		class Impl;
		std::shared_ptr<Impl> m_pImpl;

	protected:

		FontManager();

	private:

		//----------------------------------------------
		String m_id;		/// ID
		int m_fontSize = 16;		/// フォントサイズ
		//----------------------------------------------

		std::shared_ptr<IStream> m_stream;

		FontManager(const FontManager&) = delete;				// コピーコンストラクタ封じ
		FontManager& operator= (const FontManager&) = delete;	// コピーコンストラクタ封じ
		FontManager& operator= (const FontManager&&) = delete;	// ムーブセマンティクスコンストラクタ封じ
	};
}

#endif
