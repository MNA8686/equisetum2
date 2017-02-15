#include "system/System.h"
#include "system/Singleton.h"
#include "system/LoggerCompat.h"
#include "timer/SystemTimerCompat.h"
#include "input/Key.h"
#include "input/KeyboardCompat.h"
#include "input/MouseCompat.h"
#include "input/JoystickCompat.h"
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

		// �L�[�{�[�h�X�V����
		Singleton<KeyboardCompat>::GetInstance()->Prepare();
		// �}�E�X�X�V����
		Singleton<MouseCompat>::GetInstance()->Prepare();
		// �W���C�X�e�B�b�N�X�V����
		Singleton<JoystickCompat>::GetInstance()->Prepare();

		// �C�x���g���[�v
		while (SDL_PollEvent(&e))
		{
			Singleton<SDLEventManager>::GetInstance()->Dispatch(e);

			if (e.type == SDL_QUIT)
			{
				continueFlag = false;
			}
		}

		// �L�[�{�[�h�̏�Ԃ��X�V
		Singleton<KeyboardCompat>::GetInstance()->Update();
		// �}�E�X�̏�Ԃ��X�V
		Singleton<MouseCompat>::GetInstance()->Update();
		// �W���C�X�e�B�b�N�̏�Ԃ��X�V
		Singleton<JoystickCompat>::GetInstance()->Update();

		return continueFlag;
	}
}

using namespace Equisetum2;
extern void Main();

#define SDL_WINDOW_TITLE "SDL2"
#define SDL_WINDOW_WIDTH (640)
#define SDL_WINDOW_HEIGHT (360)

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

	// �V���O���g���̃C���X�^���X�쐬
	Singleton<LoggerCompat>::GetInstance();
	Singleton<SDLEventManager>::GetInstance();
	Singleton<SystemTimerCompat>::GetInstance();
	Singleton<KeyboardCompat>::GetInstance();
	Singleton<MouseCompat>::GetInstance();
	Singleton<JoystickCompat>::GetInstance();

	SDL_Window *gWindow = SDL_CreateWindow(SDL_WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOW_WIDTH,
		SDL_WINDOW_HEIGHT, 0);

	Main();

	SingletonFinalizer::Finalize();

	SDL_DestroyWindow(gWindow);
	SDL_Quit();

	return 0;
}
