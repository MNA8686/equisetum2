#include "Equisetum2.h"
#include "Object.hpp"

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>


using namespace Equisetum2;

int Main()
{
	Logger::SetPriority(LogLevel::Info);
	Logger::SetCallback([](LogLevel level, const char* str)->bool {
		return false;
	});

	auto sysTimer = SystemTimer::Create();
	auto tick = TickCounter::Create(sysTimer);
	auto fpsMaker = FpsPaceMaker::Create(tick, 60);

	auto fpsCounterTick = TickCounter::Create(sysTimer);
	auto fpsCounter = FpsCounter::Create(fpsCounterTick);

	bool pause = false;

	while (true)
	{
		// システム処理
		if (!System::Update() ||
			KB::KeyEscape.IsDown())
		{
			break;
		}

		// フルスクリーン切り替え
		static bool full = true;
		if (KB::KeyF.IsDown())
		{
			Window::SetFullscreen(full);
			full = !full;
		}

		// ポーズ切り替え
		if (KB::KeyP.IsDown())
		{
			pause = !pause;
		}

		// ポーズ機能
		if (pause)
		{
			System::Sleep(1);
			continue;
		}

		// 規定時間待ち
		while (true)
		{
			sysTimer->Update();

			uint32_t remain = fpsMaker->RemainTicks();
			
			if (remain >= 2)
			{
				System::Sleep(1);
			}
			else if (remain == 0)
			{
				break;
			}
		}

		Node::GC();

		// FPSをカウントアップ
		fpsCounter->Increase();

		Logger::OutputError(String::Sprintf("fps %d", fpsCounter->Fps()).c_str());
	}

	Object::DestroyThemAll();

	return 0;
}
