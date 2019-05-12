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
	int ret = -1;

	try
	{
		size_t count = 0;
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
							throw String::Sprintf(u8"option <-%s> syntax error!!!\n", parseOption.c_str());
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
							throw String::Sprintf("unknown option <%s>\n", opt.c_str());
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
			throw String::Sprintf("The InPath is not set.\n");
		}
		if (param.inPath[param.inPath.size() - 1] != '/' &&
			param.inPath[param.inPath.size() - 1] != '\\')
		{
			param.inPath += "/";
		}
		if (!Directory::Exists(param.inPath))
		{
			throw String::Sprintf("The InPath <%s> does not exist.\n", param.inPath.c_str());
		}

		// 出力パスチェック
		if (param.outPath.empty())
		{
			throw String::Sprintf("The OutPath is not set.\n");
		}

		// キーチェック
		if (param.key.empty())
		{
			throw String::Sprintf("The key is not set.\n");
		}

		// ファイルチェック
		if (param.vID.empty())
		{
			throw String::Sprintf("The file is not set.\n");
		}

		ret = 0;
	}
	catch (const String str)
	{
		printf("ERROR: ");
		printf(str.c_str());
	}

	return ret;
}

static std::vector<String> EnumFilesWithReplaceSeparator(const String& path)
{
	String fullPath = path;
	std::vector<String> vFileList;

	// フルパスの正規化を行う
	if (fullPath[fullPath.size() - 1] != Path::DirectorySeparatorChar[0])
	{
		fullPath += Path::DirectorySeparatorChar;
	}

	// ディレクトリ内のファイルを再帰的に列挙する
	auto optFiles = Directory::GetFiles(fullPath, true);
	if (optFiles)
	{
		vFileList = std::move(*optFiles);

		// 検索パスを取り除いてかつディレクトリの区切り文字をスラッシュにする
		for (auto& filename : vFileList)
		{
			filename = filename.substr(fullPath.size());

			if (Path::DirectorySeparatorChar != "/")
			{
				for (auto& c : filename)
				{
					if (c == Path::DirectorySeparatorChar[0])
					{
						c = '/';
					}
				}
			}
		}
	}
	
	return vFileList;
}


static int BuildArchive(const Param& param)
{
	int ret = -1;
	bool newFileCreated = false;

	try
	{
		printf("BUILDING...\n");

		auto outStream = FileStream::NewFileFromPath(param.outPath);
		if (!outStream)
		{
			throw String::Sprintf("OutPath <%s> can't create.\n", param.outPath.c_str());
		}
		
		newFileCreated = true;

		auto archiveStream = ArchivePacker::CreateFromStream(outStream, param.key);
		if (!archiveStream)
		{
			throw String::Sprintf("The ArchiveStream can't create.\n");
		}

		archiveStream->SetCrypt(1);

		for (auto& file : param.vID)
		{
			String fullPath = Path::GetFullPath(param.inPath + file);

			// ディレクトリが指定されている場合は中のファイルを列挙する
			if (Directory::Exists(fullPath))
			{
				// ディレクトリの中のファイルを列挙する
				std::vector<String> files = EnumFilesWithReplaceSeparator(fullPath);

				// 列挙したファイルをアーカイブにPUSHしていく
				for (auto& enumFile : files)
				{
					// 相対パス作成
					String filePath = file + "/" + enumFile;

					printf("PUSH %s\n", filePath.c_str());

					// 列挙したファイルをオープン
					auto inStream = FileStream::CreateFromPath(Path::GetFullPath(param.inPath) + filePath);
					if (!inStream)
					{
						throw String::Sprintf("The file <%s> can't open.\n", filePath.c_str());
					}

					// アーカイブにPUSH
					if (!archiveStream->Push(filePath, inStream))
					{
						throw String::Sprintf("The file <%s> can't push.\n", filePath.c_str());
					}
				}
			}
			else
			{
				printf("PUSH %s\n", file.c_str());

				auto inStream = FileStream::CreateFromPath(fullPath);
				if (!inStream)
				{
					throw String::Sprintf("The file <%s> can't open.\n", file.c_str());
				}

				if (!archiveStream->Push(file, inStream))
				{
					throw String::Sprintf("The file <%s> can't push.\n", file.c_str());
				}
			}
		}

		printf("FINALIZING...\n");

		if (!archiveStream->Finalize())
		{
			throw String::Sprintf("finalize failed.");
		}

		ret = 0;
		printf("OK\n");
	}
	catch (const String str)
	{
		printf("ERROR: ");
		printf(str.c_str());

		// 途中で失敗した場合は作りかけのファイルを削除する
		if(newFileCreated)
		{
			File::Delete(param.outPath);
		}
	}

	return ret;
}

static int VerifyArchive(const Param& param)
{
	int ret = -1;

	try
	{
		printf("VERIFYING...\n");

		auto stream = FileStream::CreateFromPath(param.outPath);
		if (!stream)
		{
			throw String::Sprintf("archive <%s> can't open.\n", param.outPath.c_str());
		}

		auto archive = ArchiveAccessor::CreateFromStream(stream, param.key);
		if (!archive)
		{
			throw String::Sprintf("archive <%s> can't open.\n", param.outPath.c_str());
		}

		// アーカイブのハッシュが正しいか確認する
		if (!archive->HashCheck())
		{
			throw String::Sprintf("archive <%s> hash check failed.\n", param.outPath.c_str());
		}

		// アーカイブの中のファイルを列挙する
		std::vector<ArchiveMeta> vArchiveMeta;
		archive->EnumerateFiles([&vArchiveMeta](const ArchiveMeta& meta)->bool {
			vArchiveMeta.push_back(meta);
			return false;
		});

		for (auto& meta : vArchiveMeta)
		{
			printf("VERIFY %s\n", meta.id.c_str());

			// アーカイブの中のファイルのハッシュを求める
			auto metaStream = archive->SeekByArchiveMeta(meta);
			if (!metaStream)
			{
				throw String::Sprintf("The id <%s> can't open.\n", meta.id.c_str());
			}
			Optional<SHA256Value> cmp1 = SHA256::Encrypt(metaStream);

			// 元ファイルのハッシュを求める
			String fullPath = param.inPath + meta.id;
			auto inStream = FileStream::CreateFromPath(fullPath);
			if (!inStream)
			{
				throw String::Sprintf("The file <%s> can't open.\n", fullPath.c_str());
			}
			Optional<SHA256Value> cmp2 = SHA256::Encrypt(inStream);

			// ハッシュを比較する
			if (cmp1 != cmp2)
			{
				throw String::Sprintf("The file <%s> verify error.\n", meta.id.c_str());
			}
		}
		
		ret = 0;
		printf("OK\n");
	}
	catch (const String str)
	{
		printf("ERROR: ");
		printf(str.c_str());

		// 途中で失敗した場合は作りかけのファイルを削除する
		File::Delete(param.outPath);
	}

	return ret;
}

COMMAND_DEF(archive, 
	"[-I <input directory>] [-O <output file path>] [-K <secret key>] <file|directory ...>")
{
	int ret = -1;

	try
	{
		Param param;

		ret = ParseArgument(param);
		if (ret != 0)
		{
			throw String();
		}

		ret = BuildArchive(param);
		if (ret != 0)
		{
			throw String();
		}

		ret = VerifyArchive(param);
		if (ret != 0)
		{
			throw String();
		}

		ret = 0;
		printf("SUCCESS\n");
	}
	catch (const String str)
	{
		// nop
	}

	return ret;
}

