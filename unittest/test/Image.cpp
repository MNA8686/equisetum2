#include "UnitTest.hpp"
#include "Equisetum2.h"
using namespace Equisetum2;

// イメージ読み出し＆保存テスト
TEST(Image, Save)
{
	auto inStream = FileStream::CreateFromPath(Path::GetFullPath("jiki_icon.png"));
	ASSERT_TRUE(inStream);

	auto outStream = FileStream::CreateFromPath(Path::GetFullPath("jiki_icon_out.png"), FileStream::Method::Write);
	ASSERT_TRUE(outStream);

	auto image = Image::CreateFromStream(inStream);
	ASSERT_TRUE(image);

	ASSERT_EQ(64, image->Width());
	ASSERT_EQ(16, image->Height());

	ASSERT_TRUE(image->Pitch() > 0);
	ASSERT_TRUE(image->Data());

	ASSERT_TRUE(image->SaveToStream(outStream));
}

TEST(Image, Resize)
{
	auto inStream = FileStream::CreateFromPath(Path::GetFullPath("jiki_icon.png"));
	ASSERT_TRUE(inStream);

	auto outStream = FileStream::CreateFromPath(Path::GetFullPath("jiki_icon_resize.png"), FileStream::Method::Write);
	ASSERT_TRUE(outStream);

	auto image = Image::CreateFromStream(inStream);
	ASSERT_TRUE(image);

	ASSERT_TRUE(image->Resize(300, 50));
	ASSERT_EQ(300, image->Width());
	ASSERT_EQ(50, image->Height());

	ASSERT_TRUE(image->SaveToStream(outStream));
}

TEST(Image, ResizeResizeW)
{
	auto inStream = FileStream::CreateFromPath(Path::GetFullPath("jiki_icon.png"));
	ASSERT_TRUE(inStream);

	auto image = Image::CreateFromStream(inStream);
	ASSERT_TRUE(image);

	ASSERT_FALSE(image->Resize(0, 0));
	ASSERT_FALSE(image->Resize(0, 30));
	ASSERT_FALSE(image->Resize(-1, 30));
	ASSERT_TRUE(image->Resize(65535, 30));
	ASSERT_FALSE(image->Resize(65536, 30));
	ASSERT_TRUE(image->Resize(1, 30));
}

TEST(Image, ResizeResizeH)
{
	auto inStream = FileStream::CreateFromPath(Path::GetFullPath("jiki_icon.png"));
	ASSERT_TRUE(inStream);

	auto image = Image::CreateFromStream(inStream);
	ASSERT_TRUE(image);

	ASSERT_FALSE(image->Resize(50, 0));
	ASSERT_FALSE(image->Resize(50, -1));
	ASSERT_TRUE(image->Resize(50, 65535));
	ASSERT_FALSE(image->Resize(50, 65536));
	ASSERT_TRUE(image->Resize(50, 1));
}

TEST(Image, SaveFailed)
{
	auto inStream = FileStream::CreateFromPath(Path::GetFullPath("jiki_icon.png"));
	ASSERT_TRUE(inStream);

	auto outStream = FileStream::CreateFromPath(Path::GetFullPath("jiki_icon_out.png"));
	ASSERT_TRUE(outStream);

	auto image = Image::CreateFromStream(inStream);
	ASSERT_TRUE(image);

	ASSERT_FALSE(image->SaveToStream(outStream));
}

TEST(Image, SaveFailed2)
{
	auto inStream = FileStream::CreateFromPath(Path::GetFullPath("jiki_icon.png"));
	ASSERT_TRUE(inStream);

	std::shared_ptr<FileStream> outStream;

	auto image = Image::CreateFromStream(inStream);
	ASSERT_TRUE(image);

	ASSERT_FALSE(image->SaveToStream(outStream));
}

// イメージ読み出し
TEST(Image, LoadFailed)
{
	// 画像ではないものを読み出す
	auto inStream = FileStream::CreateFromPath(Path::GetFullPath("num.txt"));
	ASSERT_TRUE(inStream);

	auto image = Image::CreateFromStream(inStream);
	ASSERT_FALSE(image);
}

TEST(Image, LoadFailed2)
{
	// nullptrを入れてみる
	std::shared_ptr<FileStream> inStream;

	auto image = Image::CreateFromStream(inStream);
	ASSERT_FALSE(image);
}

// ブランクテスト(pngから読み出したイメージをブランクにコピーし、それを保存する)
TEST(Image, Blank)
{
	auto inStream = FileStream::CreateFromPath(Path::GetFullPath("jiki_icon.png"));
	ASSERT_TRUE(inStream);

	auto outStream = FileStream::CreateFromPath(Path::GetFullPath("jiki_icon_blank.png"), FileStream::Method::Write);
	ASSERT_TRUE(outStream);

	auto imageIn = Image::CreateFromStream(inStream);
	ASSERT_TRUE(imageIn);

	auto imageBlank = Image::CreateBlank(64, 16);
	ASSERT_TRUE(imageBlank);

	ASSERT_TRUE(imageBlank->Width() == imageIn->Width());
	ASSERT_TRUE(imageBlank->Height() == imageIn->Height());

	auto inPitch = imageIn->Pitch();
	ASSERT_TRUE(inPitch > 0);
	auto inData = imageIn->Data();
	ASSERT_TRUE(inData);

	auto blankPitch = imageBlank->Pitch();
	ASSERT_TRUE(blankPitch > 0);
	auto blankData = imageBlank->Data();
	ASSERT_TRUE(blankData);

	for (uint32_t y = 0; y < imageBlank->Height(); y++)
	{
		for (uint32_t x = 0; x < imageBlank->Width(); x++)
		{
			blankData[x] = inData[x];
		}

		blankData = reinterpret_cast<Color*>(reinterpret_cast<uint8_t*>(blankData) + blankPitch);
		inData = reinterpret_cast<Color*>(reinterpret_cast<uint8_t*>(inData) + inPitch);
	}

	ASSERT_TRUE(imageBlank->SaveToStream(outStream));
}


