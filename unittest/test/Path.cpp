#include "UnitTest.hpp"
#include "Equisetum2.h"
using namespace Equisetum2;

TEST(Path, GetFileName)
{
	// Path::GetFileName
	ASSERT_EQ("test.txt", Path::GetFileName("c:\\testdayo\\test.txt"));
	ASSERT_EQ("", Path::GetFileName("c:\\testdayo\\"));
	ASSERT_EQ("testdayo", Path::GetFileName("c:\\testdayo"));
	ASSERT_EQ("testdayo", Path::GetFileName("testdayo"));
	ASSERT_EQ(".txt", Path::GetFileName(".txt"));
}

TEST(Path, GetExtension)
{
	// Path::GetExtension
	ASSERT_EQ(".txt", Path::GetExtension("c:\\testdayo\\test.txt"));
	ASSERT_EQ(".txt", Path::GetExtension("test.txt"));
	ASSERT_EQ(".txt", Path::GetExtension(".txt"));
	ASSERT_EQ("", Path::GetExtension("txt"));
	ASSERT_EQ(".", Path::GetExtension("test."));
}

TEST(Path, GetFileNameWithoutExtension)
{
	// Path::GetFileNameWithoutExtension
	ASSERT_EQ("test", Path::GetFileNameWithoutExtension("c:\\testdayo\\test.txt"));
	ASSERT_EQ("test", Path::GetFileNameWithoutExtension("test.txt"));
	ASSERT_EQ("", Path::GetFileNameWithoutExtension(".txt"));
	ASSERT_EQ("txt", Path::GetFileNameWithoutExtension("txt"));
	ASSERT_EQ("test", Path::GetFileNameWithoutExtension("test."));
}

TEST(Path, ChangeExtension)
{
	// Path::ChangeExtension
	ASSERT_EQ("c:\\testdayo\\test.dat", Path::ChangeExtension("c:\\testdayo\\test.txt", ".dat"));
	ASSERT_EQ("c:\\testdayo\\test", Path::ChangeExtension("c:\\testdayo\\test.txt", ""));
	ASSERT_EQ("test.dat", Path::ChangeExtension("test.txt", ".dat"));
	ASSERT_EQ(".dat", Path::ChangeExtension(".txt", ".dat"));
	ASSERT_EQ("txt.dat", Path::ChangeExtension("txt", ".dat"));
	ASSERT_EQ("test.dat", Path::ChangeExtension("test.", ".dat"));
	ASSERT_EQ("txt", Path::ChangeExtension("txt", ""));
	ASSERT_EQ("test", Path::ChangeExtension("test.", ""));
	ASSERT_EQ("txt", Path::ChangeExtension("txt.dat", ""));
}

TEST(Path, IsPathRooted)
{
	// Path::IsPathRooted
	ASSERT_TRUE(Path::IsPathRooted("c:\\testdayo\\test.txt"));
	ASSERT_TRUE(Path::IsPathRooted("\\\\localhost\\test.txt"));
	ASSERT_FALSE(Path::IsPathRooted("test.txt"));
}

TEST(Path, Pref)
{
	// Path::GetTempPath
	ASSERT_TRUE(Path::GetTempPath("Equinox", "Example"));

	// Path::GetCachePath
	ASSERT_TRUE(Path::GetCachePath("Equinox", "Example"));

	// Path::GetPrivateDocumentPath
	ASSERT_TRUE(Path::GetPrivateDocumentPath("Equinox", "Example"));

	// Path::GetPublicDocumentPath
	ASSERT_TRUE(Path::GetPublicDocumentPath("Equinox", "Example"));
}

