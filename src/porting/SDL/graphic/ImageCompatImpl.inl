#include "graphic/ImageCompat.hpp"
#include "system/Logger.h"
#include "system/Exception.hpp"
#include "stream/SDLBinderRWops.inl"
#include "SDL_image.h"

namespace Equisetum2
{
	class ImageCompat::Impl final
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
				if (pSurface->format->format != SDL_PIXELFORMAT_ARGB8888)
				{
					auto convSurface = SDL_ConvertSurfaceFormat(pSurface, SDL_PIXELFORMAT_ARGB8888, 0);
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

				m_width = m_pSurface->w;
				m_height = m_pSurface->h;
				m_pitch = m_pSurface->pitch;

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

			if (m_width != width ||
				m_height != height)
			{
				auto newSurface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_ARGB8888);
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
						m_width = m_pSurface->w;
						m_height = m_pSurface->h;
						m_pitch = m_pSurface->pitch;

						ret = true;
					}
				}
			}

			return ret;
		}

		uint32_t Width()
		{
			return m_width;
		}

		uint32_t Height()
		{
			return m_height;
		}

		uint32_t Pitch()
		{
			return m_pitch;
		}

		Color* Data()
		{
			return nullptr;
		}

	private:
		std::shared_ptr<SDL_Surface> m_pSurface;

		uint32_t m_width = 0;
		uint32_t m_height = 0;
		uint32_t m_pitch = 0;
	};
}

