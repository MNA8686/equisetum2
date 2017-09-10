#include "UnitTest.hpp"
#include "Equisetum2.h"
using namespace Equisetum2;

TEST(String, Sprintf)
{
	auto str = String::Sprintf(u8"ハム%sが%d匹", u8"太郎", 256);
	ASSERT_EQ(u8"ハム太郎が256匹", str);
}

TEST(String, format)
{
	String str;
	str.format(u8"ハム%sが%d匹だよ", u8"太郎", 256);
	ASSERT_EQ(u8"ハム太郎が256匹だよ", str);

	auto str2 = str.substr_by_codepoints(2, 2);
	ASSERT_EQ(u8"太郎", str2);
}

TEST(String, size)
{
	{
		String str(u8"ハム太郎");
		auto size_by_codepoints = str.size_by_codepoints();
		ASSERT_EQ(4, size_by_codepoints);

		auto size = str.size();
		ASSERT_EQ(12, size);
	}

	{
		String str(u8"𠮷野家");
		auto size_by_codepoints = str.size_by_codepoints();
		ASSERT_EQ(3, size_by_codepoints);

		auto size = str.size();
		ASSERT_EQ(10, size);
	}
}

TEST(String, string)
{
	std::string sstring(u8"ハム太郎");
	String str(sstring);
	String str2 = str;
	const std::string& str3 = str2;

	ASSERT_EQ(u8"ハム太郎", str);
}

TEST(String, u16string)
{
	String str(u8"ハム太郎");
	auto stdstr16 = str.to_u16();
	String str2(stdstr16);

	ASSERT_EQ(u8"ハム太郎", str2);
}

TEST(String, u32string)
{
	String str(u8"ハム太郎");
	auto stdstr32 = str.to_u32();
	String str2(stdstr32);

	ASSERT_EQ(u8"ハム太郎", str2);
}

TEST(String, op)
{
	String str(u8"ハム");
	String str2(u8"太郎");

	auto append = str + str2;
	ASSERT_EQ(u8"ハム太郎", append);

	append += u8"だよん";
	ASSERT_EQ(u8"ハム太郎だよん", append);

	append += str2;
	ASSERT_EQ(u8"ハム太郎だよん太郎", append);

	append = str + u8"太郎";
	ASSERT_EQ(u8"ハム太郎", append);

	append = u8"ハム" + str2;
	ASSERT_EQ(u8"ハム太郎", append);
}

TEST(String, at)
{
	String str = u8"ハム太郎";
	auto at = str.at_by_codepoints(2);
	ASSERT_EQ(u8"太", at);
}

TEST(String, trim)
{
	{
		String str = u8"  ハム太郎  ";
		auto trim = str.trim();
		ASSERT_EQ(u8"ハム太郎", trim);
	}
	{
		String str = u8"ハム太郎  ";
		auto trim = str.trim();
		ASSERT_EQ(u8"ハム太郎", trim);
	}
	{
		String str = u8"  ハム太郎";
		auto trim = str.trim();
		ASSERT_EQ(u8"ハム太郎", trim);
	}
	{
		String str = u8"	ハム太郎	";
		auto trim = str.trim();
		ASSERT_EQ(u8"ハム太郎", trim);
	}
	{
		String str = u8"	ハム太郎";
		auto trim = str.trim();
		ASSERT_EQ(u8"ハム太郎", trim);
	}
	{
		String str = u8"ハム太郎		";
			auto trim = str.trim();
		ASSERT_EQ(u8"ハム太郎", trim);
	}
	{
		String str = u8"ハム太郎";
		auto trim = str.trim();
		ASSERT_EQ(u8"ハム太郎", trim);
	}
	{
		String str = u8"ハム太郎";
		auto trim = str.trim(u8"ハム");
		ASSERT_EQ(u8"太郎", trim);
	}
}


#if 0
if (0)
{
	Logger::OutputInfo("----- begin split -----\n");
	//		String txt = u8"hamu taro dayo";
	String txt = u8",,,,,,hamu,taro,8,,dayo ";
	auto split = txt.split(u8", ", true);
	for (auto elem : split)
	{
		Logger::OutputInfo("!  [%s]\n", elem.c_str());
	}
}

if (0)
{
	String regex = "(D[a-zA-Z]*)";
	//		String regex = "(D[a-zA-Z*)";
	String text = "Dog is better than Dance Dog.";

	Logger::OutputInfo("----- begin replace -----\n");
	auto rep = Regex::ReplaceAll(text, regex, "hamu");
	Logger::OutputInfo("!  %s\n", rep.c_str());
}

{
	//		File::Delete(u8"ハム.txt");
	//		File::Move("hamu.txt", u8"ハム.txt");
}

#endif

