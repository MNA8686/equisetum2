#include "Equisetum2.h"

using namespace Equisetum2;

int Main()
{
	Logger::SetPriority(LogLevel::Info);
	Logger::SetCallback([](LogLevel level, const char* str)->bool {
		printf("%s\n", str);
		return false;
	});

	int i = 0;
	for (auto& arg : Argument())
	{
		printf("[%d] %s\n", i, arg.c_str());
		i++;
	}

	return 0;
}
