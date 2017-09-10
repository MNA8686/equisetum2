#include "UnitTest.hpp"
#include "Equisetum2.h"
using namespace Equisetum2;

TEST(File, FileControl)
{
	auto docPath = Path::GetPublicDocumentPath("Equinox", "Example");
	ASSERT_TRUE(docPath);

	auto file = *docPath + "test.txt";
	auto file2 = *docPath + "test2.txt";

	File::Delete(file);
	File::Delete(file2);

	ASSERT_FALSE(File::Exists(file));
	ASSERT_FALSE(File::Exists(file2));

	ASSERT_TRUE(File::Create(file));
	ASSERT_TRUE(File::Exists(file));

	ASSERT_TRUE(File::Move(file, file2));
	ASSERT_FALSE(File::Exists(file));
	ASSERT_TRUE(File::Exists(file2));

	ASSERT_TRUE(File::Copy(file2, file));
	ASSERT_TRUE(File::Exists(file));
	ASSERT_TRUE(File::Exists(file2));

	ASSERT_FALSE(File::Copy(file, file2));
	ASSERT_TRUE(File::Copy(file, file2, true));

	ASSERT_FALSE(File::Move(file, file2));
}

