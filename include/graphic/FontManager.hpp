#if !defined(_EQFONTMANAGER_H_)
#define _EQFONTMANAGER_H_

#include "stream/IStream.h"
#include "graphic/BitmapFont.hpp"
#include <memory>

#include <cereal/cereal.hpp>

namespace Equisetum2
{
	class FontManager
	{
	public:
		static std::shared_ptr<FontManager> CreateFromStream(std::shared_ptr<IStream> stream, int fontSize);
		std::shared_ptr<BitmapFont> MakeBitmapFont(const String& sampleStr, Color color, Size maxSize = { 1024, 1024 });

	protected:

		FontManager() = default;

	private:
		class Impl;
		std::shared_ptr<Impl> m_pImpl;

		String m_id;		/// ID

		FontManager(const FontManager&) = delete;				// コピーコンストラクタ封じ
		FontManager& operator= (const FontManager&) = delete;	// コピーコンストラクタ封じ
		FontManager& operator= (const FontManager&&) = delete;	// ムーブセマンティクスコンストラクタ封じ
	};
}

#endif
