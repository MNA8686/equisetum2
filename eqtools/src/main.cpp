﻿#include "Command.hpp"
#include "Equisetum2.h"
using namespace Equisetum2;

#if 0
// eqtools archive -I path -O path -K key ...
int DoArchive()
{
	size_t count = 0;

	String inPath;
	String outPath;
	String key;
	std::vector<String> vID;
	String parseOption;

	for (auto& arg : Argument())
	{
		if (count != 0)
		{
			if (!arg.empty())
			{
				if (arg[0] == '-')
				{
					if (!parseOption.empty())
					{
						printf(u8"option <-%s> syntax error!!!\n", parseOption.c_str());
						return -1;
					}

					auto opt = arg.substr(1, std::string::npos);
					if (opt == "I" ||
						opt == "O" ||
						opt == "K")
					{
						parseOption = opt;
					}
					else
					{
						printf("unknown option <%s>\n", opt.c_str());
						return -1;
					}
				}
				else
				{
					if (parseOption.empty())
					{
						vID.push_back(arg);
					}
					else
					{
						if (parseOption == "I")
						{
							inPath = arg;
						}
						else if (parseOption == "O")
						{
							outPath = arg;
						}
						else if (parseOption == "K")
						{
							key = arg;
						}

						parseOption.clear();
					}
				}
			}
		}

		count++;
	}

	printf("I %s\n", inPath.c_str());
	printf("O %s\n", outPath.c_str());
	printf("K %s\n", key.c_str());
	for (auto& id : vID)
	{
		printf("id %s\n", id.c_str());
	}

	return 0;
}

static const stCommand cmd = 
{
	"archive",
	"setumei\n"
	"setumei2\n"
	"setumei3",
	DoArchive
};
#endif

int Main()
{
	int ret = 0;

	Logger::SetPriority(LogLevel::Info);
	Logger::SetCallback([](LogLevel level, const char* str)->bool {
		printf("%s\n", str);
		return false;
	});

	//Command::GetInstance()->Regist(&cmd);
	return Command::GetInstance()->Do();
}
