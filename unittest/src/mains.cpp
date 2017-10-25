#include "UnitTest.hpp"
#include "Equisetum2.h"

#ifdef _MSC_VER
#include <crtdbg.h>  // debugging routines
#endif

using namespace Equisetum2;

class SingletonDeleter
{
public:
	~SingletonDeleter()
	{
		SingletonFinalizer::Finalize();
	}
};
static SingletonDeleter g_SingletonDeleter;

int main(int argc, char *argv[])
{
#ifdef _MSC_VER
	// Detect memory leaks
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif

	Logger::SetPriority(LogLevel::Info);
	Logger::SetCallback([](LogLevel level, const char* str)->bool {
		printf("%s\n", str);
		return false;
	});

	// 単体テスト実行
	UnitTest::GetInstance()->Do();

	return 0;
}
