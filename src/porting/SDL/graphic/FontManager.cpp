#include "graphic/FontManager.hpp"
#include "system/Logger.h"
#include "system/Exception.hpp"
#include "stream/SDLBinderRWops.inl"
#include <algorithm>

#include "SDL_ttf.h"

//#define __TO_FILE__
#if defined(__TO_FILE__)
#include "stream/FileStream.h"
#include "fs/Path.hpp"
#endif

namespace Equisetum2
{
	class FontManager::Impl final
	{
	public:

		std::shared_ptr<TTF_Font>& GetFont()
		{
			return m_pFont;
		}

	private:
		friend class FontManager;

		std::shared_ptr<TTF_Font> m_pFont;
	};

	FontManager::FontManager()
	{
		m_pImpl = std::make_shared<FontManager::Impl>();
	}

	std::shared_ptr<FontManager> FontManager::CreateFromStream(std::shared_ptr<IStream> stream, int fontSize)
	{
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

			auto inst = std::shared_ptr<FontManager>(new FontManager);
			if (!inst)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			// フォントサイズを設定
			inst->m_fontSize = fontSize;

			// SDL_ttfの初期化
			{
				if (TTF_WasInit() == 0)
				{
					if (TTF_Init() != 0)
					{
						EQ_THROW(u8"TTFの初期化に失敗しました。");
					}
				}

				// RWops作成
				SDL_RWops* rwops = SDLBinderRWops::CreateFromStream(stream);
				if (!rwops)
				{
					EQ_THROW(u8"rwopsの作成に失敗しました。");
				}

				// フォントオープン
				TTF_Font* ttfFont = TTF_OpenFontRW(rwops, 1, fontSize);
				if (!ttfFont)
				{
					EQ_THROW(u8"ttfFontのオープンに失敗しました。");
				}

				// TTF_Font* to shared_ptr
				inst->m_pImpl->m_pFont = std::shared_ptr<TTF_Font>(ttfFont,
					[](TTF_Font* ttfFont) {
					if (ttfFont != nullptr)
					{
						TTF_CloseFont(ttfFont);
					}
				});
				if (!inst->m_pImpl->m_pFont)
				{
					EQ_THROW(u8"フォントのインスタンス作成に失敗しました。");
				}
			}

			return inst;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return nullptr;
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
					const String u8str = std::u32string{ code };

					// レンダリング後のサイズを算出
					int w = 0;
					int h = 0;
					if (TTF_SizeUTF8(m_pImpl->m_pFont.get(), u8str.c_str(), &w, &h) != 0)
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

	std::shared_ptr<Sprite> FontManager::MakeSpriteByGlyphInfo(const std::map<char32_t, stGlyphInfo>& mapGlyphInfo, Color inColor, Size maxSize)
	{
		EQ_DURING
		{
			const int height = TTF_FontHeight(m_pImpl->m_pFont.get());	// フォントの高さ
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
					SDL_Surface *pSurface = TTF_RenderUTF8_Blended(m_pImpl->m_pFont.get(), renderString.c_str(), color);
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
				if (TTF_SizeUTF8(m_pImpl->m_pFont.get(), renderString.c_str(), &w, &h) != 0)
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
				SDL_Surface *pSurface = TTF_RenderUTF8_Blended(m_pImpl->m_pFont.get(), renderString.c_str(), color);
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

#if defined(__TO_FILE__)
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
			std::shared_ptr<Texture> fontTexture = Texture::CreateBlank(imageSize.x, imageSize.y, 0, true);
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

			// コードポイント文字列を作成
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
}
