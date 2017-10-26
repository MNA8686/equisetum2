#include "system/System.h"
#include "system/Singleton.h"
#include "system/LoggerCompat.h"
#include "graphic/WindowCompat.h"
#include "timer/SystemTimerCompat.h"
#include "input/Key.h"
#include "input/KeyboardCompat.h"
#include "input/MouseCompat.h"
#include "input/TouchCompat.h"
#include "input/JoystickCompat.h"
#include "audio/AudioCompat.hpp"
#include "SDL.h"
#include "SDLEvent.h"

#ifdef _MSC_VER
#include <crtdbg.h>  // debugging routines
#endif

namespace Equisetum2
{
	void System::Sleep(uint32_t ms)
	{
		SDL_Delay(ms);
	}

	bool System::Update()
	{
		SDL_Event e;
		bool continueFlag = true;

		// ウィンドウ更新準備
		Singleton<WindowCompat>::GetInstance()->Prepare();
		// キーボード更新準備
		Singleton<KeyboardCompat>::GetInstance()->Prepare();
		// マウス更新準備
		Singleton<MouseCompat>::GetInstance()->Prepare();
		// タッチパネル更新準備
		Singleton<TouchCompat>::GetInstance()->Prepare();
		// ジョイスティック更新準備
		Singleton<JoystickCompat>::GetInstance()->Prepare();
		// オーディオ更新準備
		Singleton<AudioCompat>::GetInstance()->Prepare();

		// イベントループ
		while (SDL_PollEvent(&e))
		{
			Singleton<SDLEventManager>::GetInstance()->Dispatch(e);

			if (e.type == SDL_QUIT)
			{
				continueFlag = false;
			}
		}

		// ウィンドウの状態を更新
		Singleton<WindowCompat>::GetInstance()->Update();
		// キーボードの状態を更新
		Singleton<KeyboardCompat>::GetInstance()->Update();
		// マウスの状態を更新
		Singleton<MouseCompat>::GetInstance()->Update();
		// タッチパネルの状態を更新
		Singleton<TouchCompat>::GetInstance()->Update();
		// ジョイスティックの状態を更新
		Singleton<JoystickCompat>::GetInstance()->Update();
		// オーディオの状態を更新
		Singleton<AudioCompat>::GetInstance()->Update();

		return continueFlag;
	}
}

using namespace Equisetum2;
extern int Main();

int main(int argc, char *argv[])
{
#ifdef _MSC_VER
	// Detect memory leaks
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
	// Use _CrtSetBreakAlloc(n) to find a specific memory leak
#endif

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	// シングルトンのインスタンス作成
	Singleton<WindowCompat>::GetInstance();
	Singleton<LoggerCompat>::GetInstance();
	Singleton<SDLEventManager>::GetInstance();
	Singleton<SystemTimerCompat>::GetInstance();
	Singleton<KeyboardCompat>::GetInstance();
	Singleton<MouseCompat>::GetInstance();
	Singleton<TouchCompat>::GetInstance();
	Singleton<JoystickCompat>::GetInstance();
	Singleton<AudioCompat>::GetInstance();

	auto ret = Main();

	SingletonFinalizer::Finalize();

	SDL_Quit();

	return ret;
}
