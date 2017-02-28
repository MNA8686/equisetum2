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

		// �E�B���h�E�X�V����
		Singleton<WindowCompat>::GetInstance()->Prepare();
		// �L�[�{�[�h�X�V����
		Singleton<KeyboardCompat>::GetInstance()->Prepare();
		// �}�E�X�X�V����
		Singleton<MouseCompat>::GetInstance()->Prepare();
		// �^�b�`�p�l���X�V����
		Singleton<TouchCompat>::GetInstance()->Prepare();
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

		// �E�B���h�E�̏�Ԃ��X�V
		Singleton<WindowCompat>::GetInstance()->Update();
		// �L�[�{�[�h�̏�Ԃ��X�V
		Singleton<KeyboardCompat>::GetInstance()->Update();
		// �}�E�X�̏�Ԃ��X�V
		Singleton<MouseCompat>::GetInstance()->Update();
		// �^�b�`�p�l���̏�Ԃ��X�V
		Singleton<TouchCompat>::GetInstance()->Update();
		// �W���C�X�e�B�b�N�̏�Ԃ��X�V
		Singleton<JoystickCompat>::GetInstance()->Update();

		return continueFlag;
	}
}

using namespace Equisetum2;
extern void Main();

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
	Singleton<WindowCompat>::GetInstance();
	Singleton<LoggerCompat>::GetInstance();
	Singleton<SDLEventManager>::GetInstance();
	Singleton<SystemTimerCompat>::GetInstance();
	Singleton<KeyboardCompat>::GetInstance();
	Singleton<MouseCompat>::GetInstance();
	Singleton<TouchCompat>::GetInstance();
	Singleton<JoystickCompat>::GetInstance();

	Main();

	SingletonFinalizer::Finalize();

	SDL_Quit();

	return 0;
}
