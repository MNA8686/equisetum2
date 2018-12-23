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
			//利用するOpenGLのバージョンを設定
//			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
//			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
			//深度バッファの設定
//			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
			//ダブルバッファの設定
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

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

		void SetTitle(const String& title)
		{
			SDL_SetWindowTitle(m_pWindow.get(), title.c_str());
		}

		void SetMinimumSize(int width, int height)
		{
			SDL_SetWindowMinimumSize(m_pWindow.get(), width, height);
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

		void SetFullscreen(bool fullScreen)
		{
			SDL_SetWindowFullscreen(m_pWindow.get(), fullScreen ? SDL_WINDOW_FULLSCREEN : 0);
		}

		void ShowAlert(const String& msg)
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "", msg.c_str(), m_pWindow.get());
		}

		const String Title()
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

		Point Pos()
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

		std::shared_ptr<SDL_Window>& GetWindowPtr()
		{
			return m_pWindow;
		}

	private:

		std::shared_ptr<SDL_Window> m_pWindow;
	};
}

