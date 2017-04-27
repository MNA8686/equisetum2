#include "system/Logger.h"
#include "graphic/WindowCompat.h"
#include "SDL.h"
//#include "SDL_image.h"

namespace Equisetum2
{

	SDL_Renderer * renderer;
	SDL_Surface * image;
	SDL_Texture * texture;
#if 0
	SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Surface * image = SDL_LoadBMP("image.bmp");
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, image);

	while (!quit)
	{
		SDL_WaitEvent(&event);

		switch (event.type)
		{
		case SDL_QUIT:
			quit = true;
			break;
		}

		//SDL_Rect dstrect = { 5, 5, 320, 240 };
		//SDL_RenderCopy(renderer, texture, NULL, &dstrect);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyTexture(texture);
	SDL_FreeSurface(image);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
#endif

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

#if 0
				renderer = SDL_CreateRenderer(m_pWindow.get(), -1, 0);
				image = IMG_Load("jiki_icon.png");
				texture = SDL_CreateTextureFromSurface(renderer, image);
#endif
			}

//			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "error!", "testdayo", m_pWindow.get());
		}

		~Impl()
		{
#if 0
			SDL_DestroyTexture(texture);
			SDL_FreeSurface(image);
			SDL_DestroyRenderer(renderer);
#endif
		}

		void Prepare()
		{
		}

		void Update()
		{
#if 0
			SDL_Rect dstrect = { 0, 0, 64, 16 };
			SDL_RenderCopy(renderer, texture, NULL, &dstrect);
//			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
#endif
		}

		void SetTitle(const String& title)
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

