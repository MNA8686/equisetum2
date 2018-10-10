#include "graphic/Image.hpp"
#include "system/Logger.h"
#include "system/Exception.hpp"
#include "stream/SDLBinderRWops.inl"
#include "SDL_ttf.h"
//#include "stream/FileStream.h"
//#include "fs/Path.hpp"
//#include <map>

namespace Equisetum2
{
	class FontManager::Impl final
	{
	public:
		Impl()
		{
		}

		bool CreateFromStream(std::shared_ptr<IStream>& stream, int fontSize)
		{
			bool ret = false;

			EQ_DURING
			{
				if (TTF_WasInit() == 0)
				{
					if (TTF_Init() != 0)
					{
						EQ_THROW(u8"TTFの初期化に失敗しました。");
					}
				}

				auto rwops = SDLBinderRWops::CreateFromStream(stream);
				if (!rwops)
				{
					EQ_THROW(u8"rwopsの作成に失敗しました。");
				}

				TTF_Font* ttfFont = TTF_OpenFontRW(rwops, 1, fontSize);
				if (!ttfFont)
				{
					EQ_THROW(u8"ttfFontのオープンに失敗しました。");
				}

				m_pFont = std::shared_ptr<TTF_Font>(ttfFont,
					[](TTF_Font* ttfFont) {
					if (ttfFont != nullptr)
					{
						TTF_CloseFont(ttfFont);
					}
				});

				if (!m_pFont)
				{
					EQ_THROW(u8"フォントのインスタンス作成に失敗しました。");
				}

				ret = true;
			}
			EQ_HANDLER
			{
				Logger::OutputError(EQ_GET_HANDLER().what());
			}
			EQ_END_HANDLER

			return ret;
		}

		std::shared_ptr<TTF_Font>& GetFont()
		{
			return m_pFont;
		}

	private:
		std::shared_ptr<TTF_Font> m_pFont;
	};
}

