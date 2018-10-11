#include "graphic/FontManager.hpp"
#include "system/Logger.h"
#include "system/Exception.hpp"

#include "stream/FileStream.h"
#include "fs/Path.hpp"
#include <algorithm>

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

	Optional<std::map<char32_t, stGlyphInfo>> FontManager::GetGlyphInfoBySampleString(const String& sampleStr)
	{
		Optional<std::map<char32_t, stGlyphInfo>> optMapGlyphInfo;

		EQ_DURING
		{
			std::map<char32_t, stGlyphInfo> mapGlyphInfo;

			// 例文を元に、必要な文字のマップを作成する
			const std::u32string u32str = sampleStr.to_u32();
			for (auto& code : u32str)
			{
				if (!(code == '\r' || code == '\n'))
				{
					String u8str = std::u32string{ code };

					// レンダリング後のサイズを算出
					int w = 0;
					int h = 0;
					if (TTF_SizeUTF8(m_pImpl->GetFont().get(), u8str.c_str(), &w, &h) != 0)
					{
						EQ_THROW(u8"フォントサイズの算出に失敗しました。");
					}

					// グリフ情報を作成し、mapに登録する(重複は許可しない)
					stGlyphInfo glyphInfo{ u8str, Size{ static_cast<int32_t>(w), static_cast<int32_t>(h) } };
					mapGlyphInfo.insert(std::make_pair(code, glyphInfo));
				}
			}

			optMapGlyphInfo = mapGlyphInfo;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return optMapGlyphInfo;
	}

	static int32_t nearPow2(int32_t n)
	{
		if (n <= 0)
		{
			return 0;
		}

		if ((n & (n - 1)) == 0)
		{
			return n;
		}

		int32_t ret = 1;
		while (n > 0)
		{
			ret <<= 1;
			n >>= 1;
		}

		return ret;
	}

	std::shared_ptr<Sprite> FontManager::MakeSpriteByGlyphInfo(const std::map<char32_t, stGlyphInfo>& mapGlyphInfo, Color inColor, Size maxSize)
	{
		EQ_DURING
		{
			const int height = TTF_FontHeight(m_pImpl->GetFont().get());	// フォントの高さ
			const SDL_Color color = { inColor.rgba8888.r, inColor.rgba8888.g, inColor.rgba8888.b };	// フォントの色

			std::vector<std::shared_ptr<Image>> vImage;		// フォントイメージ(1行につき、1イメージ)
			Point nowPos = {};	// レンダリング先の座標
			Size imageSize{ 1, 1 };	// 最終的に作成されるイメージのサイズ
			String renderString;	// レンダリングする文字列
			std::vector<stSpriteAnimAtlas> vAtlas;

			for (const auto& itr : mapGlyphInfo)
			{
				const auto& glyphInfo = itr.second;

				// 画像の最大幅をオーバーする？
				if (nowPos.x + glyphInfo.size.x > maxSize.x)
				{
					if (nowPos.x == 0)
					{
						EQ_THROW(u8"指定されたmaxSize.xがフォントの幅に対して小さすぎます。");
					}

					// 1行レンダリング
					SDL_Surface *pSurface = TTF_RenderUTF8_Blended(m_pImpl->GetFont().get(), renderString.c_str(), color);
					if (!pSurface)
					{
						EQ_THROW(u8"フォントのレンダリングに失敗しました。");
					}

					// イメージインスタンス作成
					std::shared_ptr<Image> image = Image::CreateFromNativeHandle(pSurface);
					vImage.push_back(image);

					// 画像の最大幅更新
					imageSize.x = std::max(nowPos.x, imageSize.x);
					// 次の行へ
					renderString.clear();
					nowPos.x = 0;
					nowPos.y += height;
				}

				// 隣のグリフの端っこが表示されてしまうことがあるので、グリフとグリフの間にはスペースを入れることとする。
				renderString += glyphInfo.u8str + " ";

				// 文字列を1文字ずつ増やしてレンダリングサイズを取得する
				// (1文字だけ計測した場合と文字列全体を計測した場合、プロポーショナルでは結果がずれてしまう。
				//  仕方がないので実情に応じたサイズを取得する必要がある。)
				int w = 0;
				int h = 0;
				if (TTF_SizeUTF8(m_pImpl->GetFont().get(), renderString.c_str(), &w, &h) != 0)
				{
					EQ_THROW(u8"フォントサイズの算出に失敗しました。");
				}

				vAtlas.push_back({ { nowPos.x, nowPos.y },{ glyphInfo.size.x, glyphInfo.size.y },{ 0.5f, 0.5f } });

				nowPos.x = w;
			}

			// 残っている文字列があればレンダリングする
			if (nowPos.x > 0)
			{
				// 1行レンダリング
				SDL_Surface *pSurface = TTF_RenderUTF8_Blended(m_pImpl->GetFont().get(), renderString.c_str(), color);
				if (!pSurface)
				{
					EQ_THROW(u8"フォントのレンダリングに失敗しました。");
				}

				// イメージインスタンス作成
				std::shared_ptr<Image> image = Image::CreateFromNativeHandle(pSurface);
				vImage.push_back(image);

				// 次の行へ
				imageSize.x = std::max(nowPos.x, imageSize.x);
				nowPos.y += height;
			}

			// イメージの高さを確定
			imageSize.y = nowPos.y;

			if (imageSize.y > maxSize.y)
			{
				EQ_THROW(u8"指定されたmaxSize.yを超えました。");
			}

#if 1
			// for debug
			int count = 0;
			for (auto& img : vImage)
			{
				auto stream = FileStream::NewFileFromPath(Path::GetFullPath(String::Sprintf("font%d.png", count).c_str()));
				img->SaveToStream(stream);
				count++;
			}
#endif

			// 最終出力用のテクスチャを作成
//			std::shared_ptr<Texture> fontTexture = Texture::CreateBlank(imageSize.x, imageSize.y, 0);
			std::shared_ptr<Texture> fontTexture = Texture::CreateBlank(nearPow2(imageSize.x), nearPow2(imageSize.y), 0);
			if (!fontTexture)
			{
				EQ_THROW(u8"テクスチャの作成に失敗しました。");
			}

			// 複数のイメージを1枚のテクスチャにまとめる
			Point pos;
			for (const auto& image : vImage)
			{
				fontTexture->Update(pos, image);
				pos.y += height;
			}

			std::shared_ptr<Sprite> sprite = Sprite::CreateFromTexture(fontTexture);
			if (!sprite)
			{
				EQ_THROW(u8"スプライトの作成に失敗しました。");
			}
			if (!vAtlas.empty())
			{
				sprite->SetAnimAtlas(vAtlas);
			}

			return sprite;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return nullptr;
	}

	std::shared_ptr<BitmapFont> FontManager::MakeBitmapFont(const String& sampleStr, Color inColor, Size maxSize)
	{
		EQ_DURING
		{
			// グリフ情報作成
			Optional<std::map<char32_t, stGlyphInfo>> optMapGlyphInfo = GetGlyphInfoBySampleString(sampleStr);
			if (!optMapGlyphInfo)
			{
				EQ_THROW(u8"フォント情報の作成に失敗しました。");
			}

			if ((*optMapGlyphInfo).empty())
			{
				EQ_THROW(u8"有効なグリフ情報がありません。");
			}

			// グリフ情報からスプライトを作成
			std::shared_ptr<Sprite> sprite = MakeSpriteByGlyphInfo(*optMapGlyphInfo, inColor, maxSize);
			if (!sprite)
			{
				EQ_THROW(u8"スプライトの作成に失敗しました。");
			}

			if ((*optMapGlyphInfo).size() != sprite->GetAnimAtlas().size())
			{
				EQ_THROW(u8"グリフの数とスプライトのアトラス数が一致しません。");
			}

			// コードポイントを作成
			String codePoint;
			for (const auto& info : *optMapGlyphInfo)
			{
				codePoint += info.second.u8str;
			}

			// ビットマップフォントを作成
			std::shared_ptr<BitmapFont> bitmapFont = BitmapFont::CreateFromSprite(sprite, codePoint);
			if (!bitmapFont)
			{
				EQ_THROW(u8"ビットマップフォントの作成に失敗しました。");
			}

			return bitmapFont;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return nullptr;
	}

#if 0
	std::shared_ptr<BitmapFont> FontManager::MakeBitmapFont(const String& sampleStr, Color inColor, Size maxSize)
	{
		EQ_DURING
		{
			std::shared_ptr<BitmapFont> inst;

			typedef struct
			{
				size_t pos;			// 例文の何文字目に出てきたか
				Size size;			// レンダリング時のサイズ
			}stCodeMap;
			std::map<char32_t, stCodeMap> mapCodePoint;

			// 例文を元に、必要な文字のマップを作成する
			const std::u32string u32str = sampleStr.to_u32();
			size_t pos = 0;
			for (auto& code : u32str)
			{
				if (!(code == '\r' || code == '\n'))
				{
					String u8str = std::u32string{ code };

					// レンダリング後のサイズを算出
					int w = 0;
					int h = 0;
					if (TTF_SizeUTF8(m_pImpl->GetFont().get(), u8str.c_str(), &w, &h) == 0)
					{
						stCodeMap codeMap{ pos, Size { static_cast<int32_t>(w), static_cast<int32_t>(h) } };
						mapCodePoint.insert(std::make_pair(code, codeMap));
					}
				}

				pos++;
			}

			int height = TTF_FontHeight(m_pImpl->GetFont().get());
			SDL_Color color = { inColor.rgba8888.r, inColor.rgba8888.g, inColor.rgba8888.b };
			std::vector<std::shared_ptr<Image>> vImage;

			Point nowPos;
			Size imageSize;
			String renderString;
			for (const auto& itr : mapCodePoint)
			{
				const auto& codeMap = itr.second;

				if (nowPos.x + codeMap.size.x > maxSize.x)
				{
					if (nowPos.x == 0)
					{
						EQ_THROW(u8"指定されたmaxSize.xがフォントの幅に対して小さすぎます。");
					}

					SDL_Surface *pSurface = TTF_RenderUTF8_Blended(m_pImpl->GetFont().get(), renderString.c_str(), color);
					if (!pSurface)
					{
						EQ_THROW(u8"フォントのレンダリングに失敗しました。");
					}

					std::shared_ptr<Image> image = Image::CreateFromNativeHandle(pSurface);
					vImage.push_back(image);

					imageSize.x = std::max(nowPos.x, imageSize.x);
					renderString.clear();
					nowPos.x = 0;
					nowPos.y += height;
				}

				String u8str = std::u32string{ u32str[codeMap.pos] };
				renderString += u8str;
				nowPos.x += codeMap.size.x;
			}

			if (nowPos.x > 0)
			{
				imageSize.x = std::max(nowPos.x, imageSize.x);
				nowPos.y += height;

				SDL_Surface *pSurface = TTF_RenderUTF8_Blended(m_pImpl->GetFont().get(), renderString.c_str(), color);
				if (!pSurface)
				{
					EQ_THROW(u8"フォントのレンダリングに失敗しました。");
				}
				std::shared_ptr<Image> image = Image::CreateFromNativeHandle(pSurface);
				vImage.push_back(image);
			}

			imageSize.y = nowPos.y;

			Logger::OutputDebug(String::Sprintf("size %d, %d", imageSize.x, imageSize.y).c_str());

			{
				auto fontImage = Image::CreateBlank(imageSize.x, imageSize.y);
				Point pos;

				for (auto& img : vImage)
				{
					img->CopyTo(fontImage, Rect{ 0, 0, (int32_t)img->Width(), (int32_t)img->Height() }, Rect{ pos.x, pos.y, (int32_t)img->Width(), (int32_t)img->Height() });
					pos.y += height;
				}

				auto stream = FileStream::NewFileFromPath(Path::GetFullPath("font.png"));
				fontImage->SaveToStream(stream);
			}

#if 0
			int count = 0;
			for (auto& img : vImage)
			{
				auto stream = FileStream::NewFileFromPath(Path::GetFullPath(String::Sprintf("font%d.png", count)));
				img->SaveToStream(stream);

				count++;
			}
#endif

#if 0
			TTF_SetFontOutline(m_pImpl->GetFont().get(), 1);
//			TTF_SetFontStyle(m_pImpl->GetFont().get(), TTF_STYLE_ITALIC);
			//TTF_SetFontHinting(m_pImpl->GetFont().get(), TTF_HINTING_LIGHT);
			//TTF_SetFontKerning(m_pImpl->GetFont().get(), 0);

			int height = TTF_FontHeight(m_pImpl->GetFont().get());
			Logger::OutputDebug(String::Sprintf("height %d", height).c_str());
			int skip = TTF_FontLineSkip(m_pImpl->GetFont().get());
			Logger::OutputDebug(String::Sprintf("skip %d", skip).c_str());
			int ascent = TTF_FontAscent(m_pImpl->GetFont().get());
			Logger::OutputDebug(String::Sprintf("ascent %d", ascent).c_str());
			int descent = TTF_FontDescent(m_pImpl->GetFont().get());
			Logger::OutputDebug(String::Sprintf("descent %d", descent).c_str());

			{
				int w,h;
				TTF_SizeUTF8(m_pImpl->GetFont().get(), u8"a", &w, &h);
				Logger::OutputDebug(String::Sprintf("w %d, h %d", w, h).c_str());

				TTF_SizeUTF8(m_pImpl->GetFont().get(), u8"が", &w, &h);
				Logger::OutputDebug(String::Sprintf("w %d, h %d", w, h).c_str());

				TTF_SizeUTF8(m_pImpl->GetFont().get(), u8"広", &w, &h);
				Logger::OutputDebug(String::Sprintf("w %d, h %d", w, h).c_str());
			}

			SDL_Surface *pSurface = TTF_RenderUTF8_Blended(m_pImpl->GetFont().get(), sampleStr.c_str(), color);
			if (!pSurface)
			{
				EQ_THROW(u8"フォントのレンダリングに失敗しました。");
			}

			auto image = Image::CreateFromNativeHandle(pSurface);
			auto stream = FileStream::NewFileFromPath(Path::GetFullPath("font.png"));
			image->SaveToStream(stream);
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
#endif
}
