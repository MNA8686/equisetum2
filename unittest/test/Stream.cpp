#include "UnitTest.hpp"
#include "Equisetum2.h"
using namespace Equisetum2;

TEST(PartialStream, Basic)
{
	std::vector<uint8_t> buf(256);

	uint8_t i = 0;
	for (auto& elem : buf)
	{
		elem = i;
		i++;
	}

	auto txtStream = MemoryStream::CreateFromBuf(&buf[0], buf.size());
	ASSERT_TRUE(txtStream);

	auto partial = PartialStream::CreateFromStream(txtStream, 2, 5);
	ASSERT_TRUE(partial);

	ASSERT_EQ(0, partial->Position());
	ASSERT_EQ(5, partial->Length());

	ASSERT_EQ(2, partial->ReadByte());
	ASSERT_EQ(3, partial->ReadByte());
	ASSERT_EQ(4, partial->ReadByte());
	ASSERT_EQ(5, partial->ReadByte());
	ASSERT_EQ(6, partial->ReadByte());
	ASSERT_TRUE(partial->ReadByte() < 0);
	ASSERT_TRUE(partial->ReadByte() < 0);
	ASSERT_TRUE(partial->ReadByte() < 0);
	ASSERT_EQ(5, partial->Position());

	ASSERT_TRUE(partial->WriteByte(10) <= 0);

	ASSERT_EQ(5, partial->Seek(1, SeekOrigin::Current));
	ASSERT_EQ(5, partial->Seek(0, SeekOrigin::Current));
	ASSERT_EQ(4, partial->Seek(-1, SeekOrigin::Current));
	ASSERT_EQ(3, partial->Seek(-1, SeekOrigin::Current));
	ASSERT_EQ(2, partial->Seek(-1, SeekOrigin::Current));
	ASSERT_EQ(1, partial->Seek(-1, SeekOrigin::Current));
	ASSERT_EQ(0, partial->Seek(-1, SeekOrigin::Current));
	ASSERT_EQ(0, partial->Seek(-1, SeekOrigin::Current));

	ASSERT_EQ(1, partial->Seek(1, SeekOrigin::Current));
	ASSERT_EQ(2, partial->Seek(1, SeekOrigin::Current));
	ASSERT_EQ(3, partial->Seek(1, SeekOrigin::Current));
	ASSERT_EQ(4, partial->Seek(1, SeekOrigin::Current));
	ASSERT_EQ(5, partial->Seek(1, SeekOrigin::Current));
	ASSERT_EQ(5, partial->Seek(1, SeekOrigin::Current));
	ASSERT_EQ(0, partial->Seek(-100, SeekOrigin::Current));
	ASSERT_EQ(3, partial->Seek(3, SeekOrigin::Current));
	ASSERT_EQ(5, partial->Seek(100, SeekOrigin::Current));

	ASSERT_EQ(0, partial->Seek(-1, SeekOrigin::Begin));
	ASSERT_EQ(0, partial->Seek(0, SeekOrigin::Begin));
	ASSERT_EQ(1, partial->Seek(1, SeekOrigin::Begin));
	ASSERT_EQ(5, partial->Seek(5, SeekOrigin::Begin));
	ASSERT_EQ(5, partial->Seek(6, SeekOrigin::Begin));

	ASSERT_EQ(5, partial->Seek(1, SeekOrigin::End));
	ASSERT_EQ(5, partial->Seek(0, SeekOrigin::End));
	ASSERT_EQ(4, partial->Seek(-1, SeekOrigin::End));
	ASSERT_EQ(0, partial->Seek(-5, SeekOrigin::End));
	ASSERT_EQ(0, partial->Seek(-6, SeekOrigin::End));

	std::vector<uint8_t> v(1024);
	memset(&v[0], 0x5A, v.size());

	auto readResult = partial->Read(&v[0], 100);
	ASSERT_TRUE(readResult);
	ASSERT_EQ(5, *readResult);

	ASSERT_EQ(2, v[0]);
	ASSERT_EQ(3, v[1]);
	ASSERT_EQ(4, v[2]);
	ASSERT_EQ(5, v[3]);
	ASSERT_EQ(6, v[4]);
	ASSERT_EQ(0x5A, v[5]);

	auto writeResult = partial->Write(&v[0], 100);
	ASSERT_FALSE(writeResult);
}

// 空のファイル
TEST(PartialStream, Empty)
{
	auto txtStream = FileStream::CreateFromPath(Path::GetFullPath("empty.txt"));
	ASSERT_TRUE(txtStream);

	auto partial = PartialStream::CreateFromStream(txtStream, 0, 0);
	ASSERT_TRUE(partial);

	ASSERT_EQ(0, partial->Position());
	ASSERT_EQ(0, partial->Length());

	ASSERT_TRUE(partial->ReadByte() < 0);
	ASSERT_TRUE(partial->ReadByte() < 0);
	ASSERT_TRUE(partial->ReadByte() < 0);

	ASSERT_TRUE(partial->WriteByte(10) <= 0);

	ASSERT_EQ(0, partial->Seek(1, SeekOrigin::Current));
	ASSERT_EQ(0, partial->Seek(0, SeekOrigin::Current));
	ASSERT_EQ(0, partial->Seek(-1, SeekOrigin::Current));

	ASSERT_EQ(0, partial->Seek(-1, SeekOrigin::Begin));
	ASSERT_EQ(0, partial->Seek(0, SeekOrigin::Begin));
	ASSERT_EQ(0, partial->Seek(1, SeekOrigin::Begin));

	ASSERT_EQ(0, partial->Seek(1, SeekOrigin::End));
	ASSERT_EQ(0, partial->Seek(0, SeekOrigin::End));
	ASSERT_EQ(0, partial->Seek(-1, SeekOrigin::End));

	uint8_t buf[100];
	auto readResult = partial->Read(buf, sizeof(buf));
	ASSERT_TRUE(readResult);
	ASSERT_EQ(0, *readResult);

	auto writeResult = partial->Write(buf, sizeof(buf));
	ASSERT_FALSE(writeResult);
}

// 主に境界値チェック
TEST(PartialStream, CreateFromStream)
{
	std::vector<uint8_t> buf(256);

	uint8_t i = 0;
	for (auto& elem : buf)
	{
		elem = i;
		i++;
	}

	auto txtStream = MemoryStream::CreateFromBuf(&buf[0], buf.size());
	ASSERT_TRUE(txtStream);
	ASSERT_EQ(256, txtStream->Length());

	{
		auto partial = PartialStream::CreateFromStream(txtStream, -1, 5);
		ASSERT_FALSE(partial);
	}
	{
		auto partial = PartialStream::CreateFromStream(txtStream, 0, 5);
		ASSERT_TRUE(partial);
		ASSERT_EQ(5, partial->Length());
	}

	{
		auto partial = PartialStream::CreateFromStream(txtStream, 0, 0);
		ASSERT_TRUE(partial);
		ASSERT_EQ(0, partial->Length());
	}
	{
		auto partial = PartialStream::CreateFromStream(txtStream, 0, -1);
		ASSERT_FALSE(partial);
	}

	{
		auto partial = PartialStream::CreateFromStream(txtStream, 0, 255);
		ASSERT_TRUE(partial);
		ASSERT_EQ(255, partial->Length());
	}
	{
		auto partial = PartialStream::CreateFromStream(txtStream, 0, 256);
		ASSERT_TRUE(partial);
		ASSERT_EQ(256, partial->Length());
	}
	{
		auto partial = PartialStream::CreateFromStream(txtStream, 0, 257);
		ASSERT_FALSE(partial);
	}

	{
		auto partial = PartialStream::CreateFromStream(txtStream, 10, 245);
		ASSERT_TRUE(partial);
		ASSERT_EQ(245, partial->Length());
	}
	{
		auto partial = PartialStream::CreateFromStream(txtStream, 10, 246);
		ASSERT_TRUE(partial);
		ASSERT_EQ(246, partial->Length());
	}
	{
		auto partial = PartialStream::CreateFromStream(txtStream, 10, 247);
		ASSERT_FALSE(partial);
	}
}

TEST(VariableMemoryStream, Basic)
{
	std::vector<uint8_t> buf(256);

	uint8_t i = 2;
	for (auto& elem : buf)
	{
		elem = i;
		i++;
	}

	auto vstream = VariableMemoryStream::Create();
	ASSERT_TRUE(vstream);

	ASSERT_EQ(0, vstream->Position());
	ASSERT_EQ(0, vstream->Length());

	vstream->Write(buf.data(), 5);
	vstream->Seek(0, SeekOrigin::Begin);

	ASSERT_EQ(0, vstream->Position());
	ASSERT_EQ(5, vstream->Length());

	ASSERT_EQ(2, vstream->ReadByte());
	ASSERT_EQ(3, vstream->ReadByte());
	ASSERT_EQ(4, vstream->ReadByte());
	ASSERT_EQ(5, vstream->ReadByte());
	ASSERT_EQ(6, vstream->ReadByte());
	ASSERT_TRUE(vstream->ReadByte() < 0);
	ASSERT_TRUE(vstream->ReadByte() < 0);
	ASSERT_TRUE(vstream->ReadByte() < 0);
	ASSERT_EQ(5, vstream->Position());

	ASSERT_EQ(5, vstream->Seek(1, SeekOrigin::Current));
	ASSERT_EQ(5, vstream->Seek(0, SeekOrigin::Current));
	ASSERT_EQ(4, vstream->Seek(-1, SeekOrigin::Current));
	ASSERT_EQ(3, vstream->Seek(-1, SeekOrigin::Current));
	ASSERT_EQ(2, vstream->Seek(-1, SeekOrigin::Current));
	ASSERT_EQ(1, vstream->Seek(-1, SeekOrigin::Current));
	ASSERT_EQ(0, vstream->Seek(-1, SeekOrigin::Current));
	ASSERT_EQ(0, vstream->Seek(-1, SeekOrigin::Current));

	ASSERT_EQ(1, vstream->Seek(1, SeekOrigin::Current));
	ASSERT_EQ(2, vstream->Seek(1, SeekOrigin::Current));
	ASSERT_EQ(3, vstream->Seek(1, SeekOrigin::Current));
	ASSERT_EQ(4, vstream->Seek(1, SeekOrigin::Current));
	ASSERT_EQ(5, vstream->Seek(1, SeekOrigin::Current));
	ASSERT_EQ(5, vstream->Seek(1, SeekOrigin::Current));
	ASSERT_EQ(0, vstream->Seek(-100, SeekOrigin::Current));
	ASSERT_EQ(3, vstream->Seek(3, SeekOrigin::Current));
	ASSERT_EQ(5, vstream->Seek(100, SeekOrigin::Current));

	ASSERT_EQ(0, vstream->Seek(-1, SeekOrigin::Begin));
	ASSERT_EQ(0, vstream->Seek(0, SeekOrigin::Begin));
	ASSERT_EQ(1, vstream->Seek(1, SeekOrigin::Begin));
	ASSERT_EQ(5, vstream->Seek(5, SeekOrigin::Begin));
	ASSERT_EQ(5, vstream->Seek(6, SeekOrigin::Begin));

	ASSERT_EQ(5, vstream->Seek(1, SeekOrigin::End));
	ASSERT_EQ(5, vstream->Seek(0, SeekOrigin::End));
	ASSERT_EQ(4, vstream->Seek(-1, SeekOrigin::End));
	ASSERT_EQ(0, vstream->Seek(-5, SeekOrigin::End));
	ASSERT_EQ(0, vstream->Seek(-6, SeekOrigin::End));

	std::vector<uint8_t> v(1024);
	memset(&v[0], 0x5A, v.size());

	auto readResult = vstream->Read(&v[0], 100);
	ASSERT_TRUE(readResult);
	ASSERT_EQ(5, *readResult);

	ASSERT_EQ(2, v[0]);
	ASSERT_EQ(3, v[1]);
	ASSERT_EQ(4, v[2]);
	ASSERT_EQ(5, v[3]);
	ASSERT_EQ(6, v[4]);
	ASSERT_EQ(0x5A, v[5]);

	{
		auto writeResult = vstream->Write(v.data(), 100);
		ASSERT_TRUE(writeResult);
		ASSERT_EQ(100, *writeResult);
		ASSERT_EQ(105, vstream->Position());
	}
	{
		auto writeResult = vstream->Write(v.data(), 1024);
		ASSERT_TRUE(writeResult);
		ASSERT_EQ(1024, *writeResult);
		ASSERT_EQ(105 + 1024, vstream->Position());
		ASSERT_EQ(105 + 1024, vstream->Length());
	}
	{
		ASSERT_EQ(500, vstream->Seek(500, SeekOrigin::Begin));

		auto writeResult = vstream->Write(v.data(), 1024);
		ASSERT_TRUE(writeResult);
		ASSERT_EQ(1024, *writeResult);
		ASSERT_EQ(500 + 1024, vstream->Position());
		ASSERT_EQ(500 + 1024, vstream->Length());
	}
	{
		ASSERT_EQ(100, vstream->Seek(100, SeekOrigin::Begin));

		auto writeResult = vstream->Write(v.data(), 1024);
		ASSERT_TRUE(writeResult);
		ASSERT_EQ(1024, *writeResult);
		ASSERT_EQ(100 + 1024, vstream->Position());
		ASSERT_EQ(500 + 1024, vstream->Length());
	}
	{
		ASSERT_EQ(500, vstream->Seek(500, SeekOrigin::Begin));

		auto writeResult = vstream->Write(v.data(), 1024);
		ASSERT_TRUE(writeResult);
		ASSERT_EQ(1024, *writeResult);
		ASSERT_EQ(500 + 1024, vstream->Position());
		ASSERT_EQ(500 + 1024, vstream->Length());
	}
	{
		ASSERT_EQ(501, vstream->Seek(501, SeekOrigin::Begin));

		auto writeResult = vstream->Write(v.data(), 1024);
		ASSERT_TRUE(writeResult);
		ASSERT_EQ(1024, *writeResult);
		ASSERT_EQ(501 + 1024, vstream->Position());
		ASSERT_EQ(501 + 1024, vstream->Length());
	}
	{
		std::vector<uint8_t> v(10240);

		ASSERT_EQ(0, vstream->Seek(0, SeekOrigin::Begin));
		ASSERT_EQ(0, vstream->Position());
		ASSERT_EQ(501 + 1024, vstream->Length());

		auto readResult = vstream->Read(v.data(), 10240);
		ASSERT_TRUE(readResult);
		ASSERT_EQ(501 + 1024, *readResult);
		ASSERT_EQ(501 + 1024, vstream->Position());
		ASSERT_EQ(501 + 1024, vstream->Length());
	}
}
