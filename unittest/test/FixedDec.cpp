#include "UnitTest.hpp"
#include "Equisetum2.h"
using namespace Equisetum2;

// アーカイブ作成テスト
TEST(FixedDec, standard)
{
	// int test
	{
		FixedDec val(0);
		ASSERT_EQ(0, val.GetInt());
	}
	{
		FixedDec val(1);
		ASSERT_EQ(1, val.GetInt());
		ASSERT_EQ(256, val.GetRaw());
	}
	{
		FixedDec val(100);
		ASSERT_EQ(100, val.GetInt());
	}
	{
		FixedDec val;
		val = 100;
		ASSERT_EQ(100, val.GetInt());
	}

	// raw test
	{
		FixedDec val;
		val.SetRaw(123456);
		ASSERT_EQ(123456, val.GetRaw());

		val.SetRaw(65536);
		ASSERT_EQ(256, val.GetInt());
	}

	// float
	{
		float inVal = 0.5f;
		FixedDec val(inVal);
		ASSERT_EQ(0, val.GetInt());
		ASSERT_EQ(128, val.GetRaw());
	}
	{
		float inVal = 1.f;
		FixedDec val(inVal);
		ASSERT_EQ(1, val.GetInt());
		ASSERT_EQ(256, val.GetRaw());
	}
	{
		float inVal = 3.14159f;
		FixedDec val(inVal);
		ASSERT_EQ(3, val.GetInt());
	}
	{
		FixedDec val = 3.14159f;
		ASSERT_EQ(3, val.GetInt());
	}
	{
		float inVal = 3.14159f;
		FixedDec val;
		val = inVal;
		ASSERT_EQ(3, val.GetInt());
	}

	// double
	{
		double inVal = 0.5;
		FixedDec val(inVal);
		ASSERT_EQ(0, val.GetInt());
		ASSERT_EQ(128, val.GetRaw());
	}
	{
		double inVal = 1.;
		FixedDec val(inVal);
		ASSERT_EQ(1, val.GetInt());
		ASSERT_EQ(256, val.GetRaw());
	}
	{
		double inVal = 3.14159;
		FixedDec val(inVal);
		ASSERT_EQ(3, val.GetInt());
	}
	{
		double inVal = 3.14159;
		FixedDec val;
		val = inVal;
		ASSERT_EQ(3, val.GetInt());
	}
	{
		FixedDec val = 3.14159;
		ASSERT_EQ(3, val.GetInt());
	}

	// raw test
	{
		FixedDec val = FixedDec::Raw(600);
		ASSERT_EQ(2, val.GetInt());
		ASSERT_EQ(600, val.GetRaw());
	}

	// operator test
	{
		FixedDec val(100);
		val += 300;
		ASSERT_EQ(400, val.GetInt());
	}
	{
		FixedDec val(100);
		val += FixedDec::Raw(300);
		ASSERT_EQ(101, val.GetInt());
	}
	{
		FixedDec val(100);
		auto val2 = val + FixedDec(300);
		ASSERT_EQ(400, val2.GetInt());
	}
	{
		FixedDec val(100);
		auto val2 = val + FixedDec::Raw(300);
		ASSERT_EQ(101, val2.GetInt());
	}
	{
		FixedDec val(100);
		val -= 300;
		ASSERT_EQ(-200, val.GetInt());
	}
	{
		FixedDec val(100);
		auto val2 = val - FixedDec(300);
		ASSERT_EQ(-200, val2.GetInt());
	}
	{
		FixedDec val(100);
		val *= 5;
		ASSERT_EQ(500, val.GetInt());

		// overflow check
		val = 2000000;
		val *= 4;
		ASSERT_EQ(8000000, val.GetInt());
	}
	{
		FixedDec val(100);
		auto val2 = val * FixedDec(5);
		ASSERT_EQ(500, val2.GetInt());
	}
	{
		FixedDec val(100);
		val /= 5;
		ASSERT_EQ(20, val.GetInt());

		// overflow check
		val = 8000000;
		val /= 100;
		ASSERT_EQ(80000, val.GetInt());
	}
	{
		FixedDec val(100);
		auto val2 = val / FixedDec(5);
		ASSERT_EQ(20, val2.GetInt());
	}
#if 0
	{
		FixedDec val(-2);
		val++;
		ASSERT_EQ(-1, val.GetInt());
		val++;
		ASSERT_EQ(0, val.GetInt());
		val++;
		ASSERT_EQ(1, val.GetInt());
		val++;
		ASSERT_EQ(2, val.GetInt());

		val = 3.14159;
		ASSERT_EQ(3, val.GetInt());
		val++;
		ASSERT_EQ(4, val.GetInt());
		val++;
		ASSERT_EQ(5, val.GetInt());
	}
	{
		FixedDec val(-2);
		++val;
		ASSERT_EQ(-1, val.GetInt());
		++val;
		ASSERT_EQ(0, val.GetInt());
		++val;
		ASSERT_EQ(1, val.GetInt());
		++val;
		ASSERT_EQ(2, val.GetInt());
	}
	{
		FixedDec val(2);
		--val;
		ASSERT_EQ(1, val.GetInt());
		--val;
		ASSERT_EQ(0, val.GetInt());
		--val;
		ASSERT_EQ(-1, val.GetInt());
		--val;
		ASSERT_EQ(-2, val.GetInt());
	}
#endif
	{
		FixedDec val;

		val = 0;
		ASSERT_FALSE(val);

		val = -1;
		ASSERT_TRUE(val);

		val = 1;
		ASSERT_TRUE(val);
	}
	{
		FixedDec val(100);

		ASSERT_FALSE(val == FixedDec(99));
		ASSERT_TRUE(val == FixedDec(100));
		ASSERT_FALSE(val == FixedDec(101));

		ASSERT_TRUE(val != FixedDec(99));
		ASSERT_FALSE(val != FixedDec(100));
		ASSERT_TRUE(val != FixedDec(101));
	}

	{
		FixedDec val;

		val = FixedDec::Raw(100);

		ASSERT_FALSE(val < FixedDec::Raw(99));
		ASSERT_FALSE(val < FixedDec::Raw(100));
		ASSERT_TRUE(val < FixedDec::Raw(101));

		ASSERT_FALSE(val <= FixedDec::Raw(99));
		ASSERT_TRUE(val <= FixedDec::Raw(100));
		ASSERT_TRUE(val <= FixedDec::Raw(101));

		ASSERT_TRUE(val > FixedDec::Raw(99));
		ASSERT_FALSE(val > FixedDec::Raw(100));
		ASSERT_FALSE(val > FixedDec::Raw(101));

		ASSERT_TRUE(val >= FixedDec::Raw(99));
		ASSERT_TRUE(val >= FixedDec::Raw(100));
		ASSERT_FALSE(val >= FixedDec::Raw(101));
	}

}

