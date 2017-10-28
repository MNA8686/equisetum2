#include "system/System.h"
#include "system/Singleton.h"
#include "system/LoggerCompat.h"
#include "system/ArgumentImpl.hpp"

#ifdef _MSC_VER
#include <crtdbg.h>  // debugging routines
#endif

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

	// 引数設定
	Singleton<ArgumentImpl>::GetInstance()->Set(argc, argv);

	auto ret = Main();

	SingletonFinalizer::Finalize();

	return ret;
}
