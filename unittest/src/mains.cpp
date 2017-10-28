#include "UnitTest.hpp"
#include "Equisetum2.h"

using namespace Equisetum2;

int Main()
{
	Logger::SetPriority(LogLevel::Info);
	Logger::SetCallback([](LogLevel level, const char* str)->bool {
		printf("%s\n", str);
		return false;
	});

	// 単体テスト実行
	return UnitTest::GetInstance()->Do();
}
