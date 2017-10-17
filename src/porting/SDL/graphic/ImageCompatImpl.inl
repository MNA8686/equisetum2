#include "graphic/Image.hpp"
#include "system/Logger.h"
#include "system/Exception.hpp"
#include "stream/SDLBinderRWops.inl"
#include "SDL_image.h"

namespace Equisetum2
{
	class Image::Impl final
	{
	public:
		Impl()
		{
		}

		bool InitFromStream(std::shared_ptr<IStream>& stream)
		{
			auto ret = false;

			EQ_DURING
			{
				auto rwops = SDLBinderRWops::CreateFromStream(stream);
				if (!rwops)
				{
					EQ_THROW(u8"rwopsの作成に失敗しました。");
				}

				auto pSurface = IMG_Load_RW(rwops, 1);
				if (!pSurface)
				{
					EQ_THROW(u8"サーフェスの作成に失敗しました。");
				}

				// フォーマットコンバート
				if (pSurface->format->format != SDL_PIXELFORMAT_RGBA32)
				{
					auto convSurface = SDL_ConvertSurfaceFormat(pSurface, SDL_PIXELFORMAT_RGBA32, 0);
					SDL_FreeSurface(pSurface);
					pSurface = convSurface;
				}
				if (!pSurface)
				{
					EQ_THROW(u8"サーフェスのフォーマット変換に失敗しました。");
				}

				m_pSurface = std::shared_ptr<SDL_Surface>(pSurface,
					[](SDL_Surface* pSurface) {
					if (pSurface != nullptr)
					{
						SDL_FreeSurface(pSurface);
					}
				});

				if (!m_pSurface)
				{
					EQ_THROW(u8"サーフェスのインスタンス作成に失敗しました。");
				}

				// ブレンドモードを設定
				SDL_SetSurfaceBlendMode(m_pSurface.get(), SDL_BLENDMODE_NONE);

				ret = true;
			}
			EQ_HANDLER
			{
				Logger::OutputError(EQ_GET_HANDLER().what());
			}
			EQ_END_HANDLER

			return ret;
		}

		bool InitBlank(uint32_t width, uint32_t height)
		{
			auto ret = false;

			EQ_DURING
			{
				auto pSurface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
				if (!pSurface)
				{
					EQ_THROW(u8"サーフェスの作成に失敗しました。");
				}

				m_pSurface = std::shared_ptr<SDL_Surface>(pSurface,
					[](SDL_Surface* pSurface) {
					if (pSurface != nullptr)
					{
						SDL_FreeSurface(pSurface);
					}
				});

				if (!m_pSurface)
				{
					EQ_THROW(u8"サーフェスのインスタンス作成に失敗しました。");
				}

				// ブレンドモードを設定
				SDL_SetSurfaceBlendMode(m_pSurface.get(), SDL_BLENDMODE_NONE);

				ret = true;
			}
			EQ_HANDLER
			{
				Logger::OutputError(EQ_GET_HANDLER().what());
			}
			EQ_END_HANDLER

			return ret;
		}

		bool SaveToStream(std::shared_ptr<IStream> stream)
		{
			auto ret = false;

			if (auto rwops = SDLBinderRWops::CreateFromStream(stream))
			{
				ret = IMG_SavePNG_RW(m_pSurface.get(), rwops, 1) == 0;
			}

			return ret;
		}

		bool Resize(uint32_t width, uint32_t height)
		{
			auto ret = false;

			auto newSurface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
			if (newSurface)
			{
				SDL_BlitScaled(m_pSurface.get(), nullptr, newSurface, nullptr);

				m_pSurface = std::shared_ptr<SDL_Surface>(newSurface,
					[](SDL_Surface* pSurface) {
					if (pSurface != nullptr)
					{
						SDL_FreeSurface(pSurface);
					}
				});

				if (m_pSurface)
				{
					// ブレンドモードを設定
					SDL_SetSurfaceBlendMode(m_pSurface.get(), SDL_BLENDMODE_NONE);
					ret = true;
				}
			}

			return ret;
		}

		bool CopyTo(std::shared_ptr<Image> dstImage, const Optional<Rect> src = std::experimental::nullopt, const Optional<Rect> dst = std::experimental::nullopt)
		{
			SDL_Rect srcrect;
			SDL_Rect dstrect;

			if (src)
			{
				auto& ref = *src;
				srcrect.x = ref.x;
				srcrect.y = ref.y;
				srcrect.w = ref.width;
				srcrect.h = ref.height;
			}

			if (dst)
			{
				auto& ref = *dst;
				dstrect.x = ref.x;
				dstrect.y = ref.y;
				dstrect.w = ref.width;
				dstrect.h = ref.height;
			}

			auto pDstSurface = dstImage->m_pImpl->GetSurface();
			return SDL_BlitScaled(m_pSurface.get(), src ? &srcrect : nullptr, pDstSurface.get(), dst ? &dstrect : nullptr) == 0;
		}

		uint32_t Width()
		{
			return m_pSurface->w;
		}

		uint32_t Height()
		{
			return m_pSurface->h;
		}

		uint32_t Pitch()
		{
			return m_pSurface->pitch;
		}

		Color* Data()
		{
			return reinterpret_cast<Color*>(m_pSurface->pixels);
		}

		std::shared_ptr<SDL_Surface>& GetSurface()
		{
			return m_pSurface;
		}

	private:
		std::shared_ptr<SDL_Surface> m_pSurface;
	};
}

