#include "UnitTest.hpp"
#include "Equisetum2.h"
using namespace Equisetum2;

static const char* archiveName = "test.rom";
static const char* secretKey = "EquinoxDevelopment";

//#define USE_LARGE_FILE

// アーカイブ作成テスト
TEST(Archive, MakeArchive)
{
	auto outStream = FileStream::NewFileFromPath(Path::GetFullPath(archiveName));
	ASSERT_TRUE(outStream);

	auto packer = ArchivePacker::CreateFromStream(outStream, secretKey);
	ASSERT_TRUE(outStream);

	ASSERT_TRUE(packer->SetCrypt(0));

	{
		const String id = "jiki_icon.png";
		auto in = FileStream::CreateFromPath(Path::GetFullPath(id));
		ASSERT_TRUE(in);

		Logger::OutputError(in->Url().c_str());

		ASSERT_TRUE(packer->Push(id, in));
	}

	ASSERT_TRUE(packer->SetCrypt(1));

#if defined(USE_LARGE_FILE)
	{
		const String id = "2G.vmem";
		auto in = FileStream::CreateFromPath(Path::GetFullPath(id));
		ASSERT_TRUE(in);

		ASSERT_TRUE(packer->Push(id, in));
	}
#endif

	{
		const String id = "jiki_icon.bmp";
		auto in = FileStream::CreateFromPath(Path::GetFullPath(id));
		ASSERT_TRUE(in);

		Logger::OutputError(in->Url().c_str());

		ASSERT_TRUE(packer->Push(id, in));
	}

	{
		const String id = "empty.txt";
		auto in = FileStream::CreateFromPath(Path::GetFullPath(id));
		ASSERT_TRUE(in);

		Logger::OutputError(in->Url().c_str());

		ASSERT_TRUE(packer->Push(id, in));
	}

	{
		const String id = "num.txt";
		auto in = FileStream::CreateFromPath(Path::GetFullPath(id));
		ASSERT_TRUE(in);

		Logger::OutputError(in->Url().c_str());

		ASSERT_TRUE(packer->Push(id, in));
	}

	{
		const String id = "dir/testdayo.txt";
		auto in = FileStream::CreateFromPath(Path::GetFullPath(id));
		ASSERT_TRUE(in);

		Logger::OutputError(in->Url().c_str());

		ASSERT_TRUE(packer->Push(id, in));
	}

	ASSERT_TRUE(packer->Finalize());
}

// アーカイブのHMACが正しいかチェック
TEST(Archive, Check)
{
	// OK
	{
		auto inStream = FileStream::CreateFromPath(Path::GetFullPath(archiveName));
		ASSERT_TRUE(inStream);

		auto archiveStream = ArchiveAccessor::CreateFromStream(inStream, secretKey);
		ASSERT_TRUE(archiveStream);

		auto check = archiveStream->HashCheck();
		ASSERT_TRUE(check);
	}

	// NG 
	{
		auto inStream = FileStream::CreateFromPath(Path::GetFullPath(archiveName));
		ASSERT_TRUE(inStream);

		auto archiveStream = ArchiveAccessor::CreateFromStream(inStream, "tekitou");
		ASSERT_TRUE(archiveStream);

		auto check = archiveStream->HashCheck();
		ASSERT_FALSE(check);
	}
}

// アーカイブから取り出したファイルが正しいかチェック
TEST(Archive, verify)
{
	auto inStream = FileStream::CreateFromPath(Path::GetFullPath(archiveName));
	ASSERT_TRUE(inStream);

	auto archiveStream = ArchiveAccessor::CreateFromStream(inStream, secretKey);
	ASSERT_TRUE(archiveStream);

#if defined(USE_LARGE_FILE)
	{
		const String id = "2G.vmem";
		auto cmp = FileStream::CreateFromPath(Path::GetFullPath(id));
		ASSERT_TRUE(cmp);

		auto partial = archiveStream->FindByID(id);
		ASSERT_TRUE(partial);

		ASSERT_TRUE(cmp->Length() == partial->Length());

		auto error = false;
		std::vector<uint8_t> src(1 * 1024 * 1024);
		std::vector<uint8_t> dst(1 * 1024 * 1024);
		while (true)
		{
			auto optSrc = cmp->Read(&src[0], src.size());
			auto optDst = partial->Read(&dst[0], dst.size());
			if (!optSrc || !optDst)
			{
				error = true;
				break;
			}

			if (*optSrc == 0 && *optDst == 0)
			{
				break;
			}
			
			if (memcmp(&src[0], &dst[0], src.size()) != 0)
			{
				error = true;
				break;
			}

//			auto str = String::Sprintf(u8"  progress %lld", cmp->Position());
//			Logger::OutputCritical(str.c_str());
		}

		ASSERT_FALSE(error);
	}
#endif

	{
		const String id = "jiki_icon.png";
		auto cmp = FileStream::CreateFromPath(Path::GetFullPath(id));
		ASSERT_TRUE(cmp);

		auto partial = archiveStream->FindByID(id);
		ASSERT_TRUE(partial);

		Logger::OutputError(partial->Url().c_str());

		ASSERT_TRUE(cmp->Length() == partial->Length());

		auto error = false;
		for (int64_t i = 0; i < cmp->Length(); i++)
		{
			if (cmp->ReadByte() != partial->ReadByte())
			{
				error = true;
				break;
			}
		}

		ASSERT_FALSE(error);
	}

	{
		const String id = "jiki_icon.bmp";
		auto cmp = FileStream::CreateFromPath(Path::GetFullPath(id));
		ASSERT_TRUE(cmp);

		auto partial = archiveStream->FindByID(id);
		ASSERT_TRUE(partial);

		Logger::OutputError(partial->Url().c_str());

		ASSERT_TRUE(cmp->Length() == partial->Length());

		auto error = false;
		for (int64_t i = 0; i < cmp->Length(); i++)
		{
			if (cmp->ReadByte() != partial->ReadByte())
			{
				error = true;
				break;
			}
		}

		ASSERT_FALSE(error);
	}

	{
		const String id = "num.txt";
		auto cmp = FileStream::CreateFromPath(Path::GetFullPath(id));
		ASSERT_TRUE(cmp);

		auto partial = archiveStream->FindByID(id);
		ASSERT_TRUE(partial);

		Logger::OutputError(partial->Url().c_str());

		ASSERT_TRUE(cmp->Length() == partial->Length());

		auto error = false;
		for (int64_t i = 0; i < cmp->Length(); i++)
		{
			if (cmp->ReadByte() != partial->ReadByte())
			{
				error = true;
				break;
			}
		}

		ASSERT_FALSE(error);
	}

	{
		const String id = "dir/testdayo.txt";
		auto cmp = FileStream::CreateFromPath(Path::GetFullPath(id));
		ASSERT_TRUE(cmp);

		auto partial = archiveStream->FindByID(id);
		ASSERT_TRUE(partial);

		Logger::OutputError(partial->Url().c_str());

		ASSERT_TRUE(cmp->Length() == partial->Length());

		auto error = false;
		for (int64_t i = 0; i < cmp->Length(); i++)
		{
			if (cmp->ReadByte() != partial->ReadByte())
			{
				error = true;
				break;
			}
		}

		ASSERT_FALSE(error);
	}

	{
		const String id = "empty.txt";

		auto partial = archiveStream->FindByID(id);
		ASSERT_TRUE(partial);

		ASSERT_EQ(0, partial->Length());
		ASSERT_TRUE(partial->ReadByte() < 0);
	}

	// 存在しないファイルを指定
	{
		const String id = "";
		auto partial = archiveStream->FindByID(id);
		ASSERT_FALSE(partial);
	}
	{
		const String id = "notfound";
		auto partial = archiveStream->FindByID(id);
		ASSERT_FALSE(partial);
	}
}

// メモリストリームの途中からアーカイブを書き込んでも正しく作成されるかテスト
TEST(Archive, MemoryStream)
{
	const int64_t begin = 150;

	std::vector<uint8_t> buf(10 * 1024 * 1024);
	memset(&buf[0], 0x5A, buf.size());

	auto outStream = MemoryStream::CreateFromBuf(&buf[0], buf.size());
	ASSERT_TRUE(outStream);

	ASSERT_EQ(begin, outStream->Seek(begin, SeekOrigin::Begin));

	auto packer = ArchivePacker::CreateFromStream(outStream, secretKey);
	ASSERT_TRUE(outStream);

	ASSERT_TRUE(packer->SetCrypt(0));

	{
		const String id = "jiki_icon.png";
		auto in = FileStream::CreateFromPath(Path::GetFullPath(id));
		ASSERT_TRUE(in);

		ASSERT_TRUE(packer->Push(id, in));
	}

	ASSERT_TRUE(packer->SetCrypt(1));

	{
		const String id = "jiki_icon.bmp";
		auto in = FileStream::CreateFromPath(Path::GetFullPath(id));
		ASSERT_TRUE(in);

		ASSERT_TRUE(packer->Push(id, in));
	}

	{
		const String id = "num.txt";
		auto in = FileStream::CreateFromPath(Path::GetFullPath(id));
		ASSERT_TRUE(in);

		ASSERT_TRUE(packer->Push(id, in));
	}

	ASSERT_TRUE(packer->Finalize());

	auto end = outStream->Position();

	// メモリ上に作ったアーカイブが正しいかチェック
	{
		auto memArchive = PartialStream::CreateFromStream(outStream, begin, end - begin);
		ASSERT_TRUE(memArchive);

		auto archiveStream = ArchiveAccessor::CreateFromStream(memArchive, secretKey);
		ASSERT_TRUE(archiveStream);

		auto check = archiveStream->HashCheck();
		ASSERT_TRUE(check);
	}

	// メモリ上に作ったアーカイブから取り出したファイルが正しいかチェック
	{
		ASSERT_EQ(begin, outStream->Seek(begin, SeekOrigin::Begin));

		const String id = "jiki_icon.bmp";
		auto cmp = FileStream::CreateFromPath(Path::GetFullPath(id));
		ASSERT_TRUE(cmp);

		auto archiveStream = ArchiveAccessor::CreateFromStream(outStream, secretKey);
		ASSERT_TRUE(archiveStream);

		auto partial = archiveStream->FindByID(id);
		ASSERT_TRUE(partial);

		Logger::OutputError(partial->Url().c_str());

		ASSERT_TRUE(cmp->Length() == partial->Length());

		auto error = false;
		for (int64_t i = 0; i < cmp->Length(); i++)
		{
			if (cmp->ReadByte() != partial->ReadByte())
			{
				error = true;
				break;
			}
		}

		ASSERT_FALSE(error);
	}
}