#include "UnitTest.hpp"
#include "Equisetum2.h"

using namespace Equisetum2;

void Main()
{
	Logger::SetPriority(LogLevel::Info);
	Logger::SetCallback([](LogLevel level, const char* str)->bool {
		return false;
	});

	// 単体テスト実行
	UnitTest::GetInstance()->Do();

	while (true)
	{
		if (!System::Update())
		{
			break;
		}

		System::Sleep(1);
	}
}
