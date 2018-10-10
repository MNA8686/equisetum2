#include "graphic/FontManager.hpp"
#include "system/Logger.h"
#include "system/Exception.hpp"

#include "stream/FileStream.h"
#include "fs/Path.hpp"
#include <map>

#include "graphic/FontManagerCompatImpl.inl"

namespace Equisetum2
{
	std::shared_ptr<FontManager> FontManager::CreateFromStream(std::shared_ptr<IStream> stream, int fontSize)
	{
		std::shared_ptr<FontManager> inst;

		EQ_DURING
		{
			if (!stream)
			{
				EQ_THROW(u8"有効なストリームではありません。");
			}

			if (!stream->CanRead())
			{
				EQ_THROW(u8"リード属性が必要です。");
			}

			auto inst_ = std::shared_ptr<FontManager>(new FontManager);
			if (!inst_)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			inst_->m_pImpl = std::make_shared<FontManager::Impl>();
			if (!inst_->m_pImpl)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			if (!inst_->m_pImpl->CreateFromStream(stream, fontSize))
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			inst = inst_;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return inst;
	}

	std::shared_ptr<BitmapFont> FontManager::MakeBitmapFont(const String& sampleStr, Color color, Size maxSize)
	{
		EQ_DURING
		{
			std::shared_ptr<BitmapFont> inst;

			typedef struct
			{
				char32_t codePoint;	// コードポイント
				size_t pos;			// 例文の何文字目に出てきたか
			}stCodeMap;
			std::map<char32_t, size_t> mapCodePoint;

			// 例文を元に、必要な文字のマップを作成する
			const std::u32string u32str = sampleStr.to_u32();
			size_t pos = 0;
			for (auto& code : u32str)
			{
				mapCodePoint.insert(std::make_pair(code, pos));
				pos++;
			}

			for (const auto& itr : mapCodePoint)
			{
				String utf8 = sampleStr.at_by_codepoints(itr.second);
			}

#if 0
			SDL_Color color = { 255, 255, 255 };
			SDL_Surface *pSurface = TTF_RenderUTF8_Blended(m_pImpl->GetFont().get(), sampleStr.c_str(), color);
			if (!pSurface)
			{
				EQ_THROW(u8"フォントのレンダリングに失敗しました。");
			}
#endif
			return inst;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return nullptr;
	}
}
