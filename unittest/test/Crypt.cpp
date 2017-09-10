#include "UnitTest.hpp"
#include "Equisetum2.h"
using namespace Equisetum2;

TEST(SHA256, Encrypt)
{
	// SHA256::Encrypt
	{
		auto str = "The quick brown fox jumps over the lazy dog";
		auto mem = MemoryStream::CreateFromBuf(str, strlen(str));
		ASSERT_TRUE(mem);

		auto opt = SHA256::Encrypt(mem);
		ASSERT_TRUE(opt);

		ASSERT_EQ("D7A8FBB307D7809469CA9ABCB0082E4F8D5651E46D3CDB762D02D0BF37C9E592", (*opt).ToString());
	}
	{
		auto str = "0123456789012345678901234567890123456789012345678901234567890123";	// 64バイト
		auto mem = MemoryStream::CreateFromBuf(str, strlen(str));
		ASSERT_TRUE(mem);

		auto opt = SHA256::Encrypt(mem);
		ASSERT_TRUE(opt);

		ASSERT_EQ("9674D9E078535B7CEC43284387A6EE39956188E735A85452B0050B55341CDA56", (*opt).ToString());
	}
	{
		auto str = "01234567890123456789012345678901234567890123456789012345678901234";	// 65バイト
		auto mem = MemoryStream::CreateFromBuf(str, strlen(str));
		ASSERT_TRUE(mem);

		auto opt = SHA256::Encrypt(mem);
		ASSERT_TRUE(opt);

		ASSERT_EQ("52774B57C10E45040A61C14D35C1C8EBEFE880082313AA0A21EBB077734CD067", (*opt).ToString());
	}
	{
		auto str = "The quick brown fox jumps over the lazy dogThe quick brown fox jumps over the lazy dogThe quick brown fox jumps over the lazy dogThe quick brown fox jumps over the lazy dog";	// 長いの
		auto mem = MemoryStream::CreateFromBuf(str, strlen(str));
		ASSERT_TRUE(mem);

		auto opt = SHA256::Encrypt(mem);
		ASSERT_TRUE(opt);

		ASSERT_EQ("1B16924BC56351CE91AC042ED7A2E75F985B03968CE15392B9B05CD6C6367C7E", (*opt).ToString());
	}
}

TEST(SHA256, HMAC)
{
	// SHA256::HMAC
	{
		auto str = "The quick brown fox jumps over the lazy dog";
		auto mem = MemoryStream::CreateFromBuf(str, strlen(str));
		ASSERT_TRUE(mem);

		auto opt = SHA256::HMAC(mem, "Equinox");
		ASSERT_TRUE(opt);

		ASSERT_EQ("C3A78C2BEDF642E3754CC3E9E1BD88275B0E491B191EE8DEA7298D027C651F9B", (*opt).ToString());
	}
	{
		auto str = "The quick brown fox jumps over the lazy dog";
		auto mem = MemoryStream::CreateFromBuf(str, strlen(str));
		ASSERT_TRUE(mem);

		auto opt = SHA256::HMAC(mem, "The quick brown fox jumps over the lazy dogThe quick brown fox jx");	// 65バイト
		ASSERT_TRUE(opt);

		ASSERT_EQ("734D92B6CDE39DD87E8B2952EA6B198D154B168A88491D52323742624BFE594E", (*opt).ToString());
	}
	{
		auto str = "The quick brown fox jumps over the lazy dog";
		auto mem = MemoryStream::CreateFromBuf(str, strlen(str));
		ASSERT_TRUE(mem);

		auto opt = SHA256::HMAC(mem, "asdjlasfsjlafjkzxm,vzxcvn,.zxcviouqwerqwreouiqwerjhasfsadf,mmmm.zvxczxcvzxcvasfdjlk;qworeuiqrweqwrwrouiqwrqowuirasdfljkasdfsjkl;fsn,n,zvzvxczn,vvvvvxzxv,xv,na1234798015671234782342342342345-0aa-f0a9sf-fas9-^ljkasdafsk;lfasdafjsdlkasfdasfdlkafsd");	//長いの
		ASSERT_TRUE(opt);

		ASSERT_EQ("A304AB116D5D0ACC591337E47D1E7982AF07C9620A901B15B70769195279BB52", (*opt).ToString());
	}
}


