#include "UnitTest.hpp"
#include "Equisetum2.h"

using namespace Equisetum2;

int Main()
{
	Logger::SetPriority(LogLevel::Info);
	Logger::SetCallback([](LogLevel level, const char* str)->bool {
		return false;
	});

	// 単体テスト実行
	auto ret = UnitTest::GetInstance()->Do();
	if (ret != 0)
	{
		return ret;
	}

	while (true)
	{
		if (!System::Update())
		{
			break;
		}

		System::Sleep(1);
	}

	return 0;
}
