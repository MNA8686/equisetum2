#include "Command.hpp"
#include "Equisetum2.h"
using namespace Equisetum2;

struct Param
{
	String inPath;
	String outPath;
	String key;
	std::vector<String> vID;
};

static int ParseArgument(Param& param)
{
	size_t count = 0;
	String parseOption;
	int ret = -1;

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
						goto ERROR;
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
						goto ERROR;
					}
				}
				else
				{
					// オプションの引数解析中以外はIDの指定と見なす
					if (parseOption.empty())
					{
						param.vID.push_back(arg);
					}
					else
					{
						if (parseOption == "I")
						{
							param.inPath = arg;
						}
						else if (parseOption == "O")
						{
							param.outPath = arg;
						}
						else if (parseOption == "K")
						{
							param.key = arg;
						}

						// オプション解析状態をクリア
						parseOption.clear();
					}
				}
			}
		}

		count++;
	}

#if 0
	// for debug
	printf("I %s\n", inPath.c_str());
	printf("O %s\n", outPath.c_str());
	printf("K %s\n", key.c_str());
	for (auto& id : vID)
	{
		printf("id %s\n", id.c_str());
	}
#endif

	// 入力パスチェック
	if (param.inPath.empty())
	{
		printf("The InPath is not set.\n");
		goto ERROR;
	}
	if (param.inPath[param.inPath.size() - 1] != '/' && 
		param.inPath[param.inPath.size() - 1] != '\\')
	{
		param.inPath += "/";
	}
	if (!Directory::Exists(param.inPath))
	{
		printf("The InPath <%s> does not exist.\n", param.inPath.c_str());
		goto ERROR;
	}

	// 出力パスチェック
	if (param.outPath.empty())
	{
		printf("The OutPath is not set.\n");
		goto ERROR;
	}

	// キーチェック
	if (param.key.empty())
	{
		printf("The key is not set.\n");
		goto ERROR;
	}

	// ファイルチェック
	if (param.vID.empty())
	{
		printf("The file is not set.\n");
		goto ERROR;
	}

	ret = 0;

ERROR:

	return ret;
}

static int BuildArchive(const Param& param)
{
	int ret = -1;
	bool newFileCreated = false;

	{

		auto outStream = FileStream::NewFileFromPath(param.outPath);
		if (!outStream)
		{
			printf("OutPath <%s> can't create.\n", param.outPath.c_str());
			goto ERROR;
		}
		
		newFileCreated = true;

		auto archiveStream = ArchivePacker::CreateFromStream(outStream, param.key);
		if (!archiveStream)
		{
			printf("The ArchiveStream can't create.\n");
			goto ERROR;
		}

		archiveStream->SetCrypt(1);

		for (auto& file : param.vID)
		{
			String fullPath = param.inPath + file;

			// ディレクトリが指定されている場合は中のファイルを列挙する
			if (Directory::Exists(fullPath))
			{
				Optional<std::vector<String>> opt = Directory::GetFiles(fullPath);
				if (!opt)
				{
					printf("directory <%s> enum files failed.", file.c_str());
					goto ERROR;
				}

				for (auto& enumFile : *opt)
				{
					auto inStream = FileStream::CreateFromPath(enumFile);
					if (!inStream)
					{
						printf("The file <%s> can't open.\n", enumFile.c_str());
						goto ERROR;
					}

					if (!archiveStream->Push(file + "/" + Path::GetFileName(enumFile), inStream))
					{
						printf("The file <%s> can't push.\n", enumFile.c_str());
						goto ERROR;
					}
				}
			}
			else
			{
				auto inStream = FileStream::CreateFromPath(fullPath);
				if (!inStream)
				{
					printf("The file <%s> can't open.\n", file.c_str());
					goto ERROR;
				}

				if (!archiveStream->Push(file, inStream))
				{
					printf("The file <%s> can't push.\n", file.c_str());
					goto ERROR;
				}
			}
		}

		if (!archiveStream->Finalize())
		{
			printf("finalize failed.");
			goto ERROR;
		}
	}

	ret = 0;

ERROR:

	// 途中で失敗した場合は作りかけのファイルを削除する
	if (ret != 0 &&
		newFileCreated)
	{
		File::Delete(param.outPath);
	}

	return ret;
}

static int VerifyArchive(const Param& param)
{
	int ret = -1;

	{
		auto stream = FileStream::CreateFromPath(param.outPath);
		if (!stream)
		{
			printf("archive <%s> can't open.\n", param.outPath.c_str());
			goto ERROR;
		}

		auto archivePath = ArchiveAccessor::CreateFromStream(stream, param.key);
		if (!archivePath)
		{
			printf("archive <%s> can't open.\n", param.outPath.c_str());
			goto ERROR;
		}

		if (!archivePath->HashCheck())
		{
			printf("archive <%s> hash check failed.\n", param.outPath.c_str());
			goto ERROR;
		}
	}

	ret = 0;

ERROR:

	// 途中で失敗した場合は作りかけのファイルを削除する
	if (ret != 0)
	{
		File::Delete(param.outPath);
	}

	return ret;
}

COMMAND_DEF(archive, 
	"[-I <input directory>] [-O <output file path>] [-K <secret key>] <file|directory ...>")
{
	Param param;
	int ret = -1;

	ret = ParseArgument(param);
	if (ret != 0)
	{
		goto ERROR;
	}

	ret = BuildArchive(param);
	if (ret != 0)
	{
		goto ERROR;
	}

	ret = VerifyArchive(param);
	if (ret != 0)
	{
		goto ERROR;
	}

	ret = 0;
	printf("ok.\n");

ERROR:

	return ret;
}

