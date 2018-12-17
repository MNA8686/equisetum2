#include "Command.hpp"
#include "Equisetum2.h"
using namespace Equisetum2;

int Main()
{
	int ret = 0;

	Logger::SetPriority(LogLevel::Info);
	Logger::SetCallback([](LogLevel level, const char* str)->bool {
		printf("%s\n", str);
		return false;
	});

	return Command::GetInstance()->Do();
}
