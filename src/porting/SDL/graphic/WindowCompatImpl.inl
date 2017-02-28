#include "system/Logger.h"
#include "graphic/WindowCompat.h"
#include "SDL.h"

namespace Equisetum2
{
	class WindowCompat::Impl final
	{
	public:
		Impl()
		{
			auto pWindow = SDL_CreateWindow(
				"",							// ウィンドウのタイトル
				SDL_WINDOWPOS_CENTERED,		// X座標の初期値
				SDL_WINDOWPOS_CENTERED,		// Y座標の初期値
				640,						// 幅のピクセル数
				480,						// 高さのピクセル数
				SDL_WINDOW_OPENGL			// フラグ
			);

			if (pWindow)
			{
				m_pWindow = std::shared_ptr<SDL_Window>(pWindow,
					[](SDL_Window* pWindow) {
					if (pWindow != nullptr)
					{
						SDL_DestroyWindow(pWindow);
					}
				});
			}
		}

		void Prepare()
		{
		}

		void Update()
		{
		}

		void SetTitle(const std::string& title)
		{
			SDL_SetWindowTitle(m_pWindow.get(), title.c_str());
		}

		void SetSize(int width, int height)
		{
			SDL_SetWindowSize(m_pWindow.get(), width, height);
		}

		void SetStyle(WindowStyle style)
		{
			if (style == WindowStyle::Fixed)
			{
				SDL_SetWindowResizable(m_pWindow.get(), SDL_FALSE);
			}
			else if (style == WindowStyle::Sizeable)
			{
				SDL_SetWindowResizable(m_pWindow.get(), SDL_TRUE);
			}
		}

		void SetPos(int x, int y)
		{
			SDL_SetWindowPosition(m_pWindow.get(), x, y);
		}

		void Centering()
		{
			SDL_SetWindowPosition(m_pWindow.get(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
		}

		const std::string Title()
		{
			return SDL_GetWindowTitle(m_pWindow.get());
		}

		Equisetum2::Size Size()
		{
			int width = 0;
			int height = 0;

			SDL_GetWindowSize(m_pWindow.get(), &width, &height);

			return{ width, height };
		}

		Point<int> Pos()
		{
			int x = 0;
			int y = 0;

			SDL_GetWindowPosition(m_pWindow.get(), &x, &y);

			return{ x, y };
		}

		WindowStyle Style()
		{
			auto style = WindowStyle::Fixed;
			auto flags = SDL_GetWindowFlags(m_pWindow.get());
			if (flags & SDL_WINDOW_RESIZABLE)
			{
				style = WindowStyle::Sizeable;
			}
			return style;
		}

	private:

		std::shared_ptr<SDL_Window> m_pWindow;
	};
}

