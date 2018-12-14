#include "Command.hpp"
#include "Equisetum2.h"
using namespace Equisetum2;

// eqtools archive -I path -O path -K key ...
COMMAND_DEF(archive, "tsukaikata")
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
				// オプション？
				if (arg[0] == '-')
				{
					// オプション解析中ならエラーとする
					if (!parseOption.empty())
					{
						printf(u8"option <-%s> syntax error!!!\n", parseOption.c_str());
						return -1;
					}

					// オプション名を抜き出す
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
					// オプションの引数解析中以外はIDの指定と見なす
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

						// オプション解析状態をクリア
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

