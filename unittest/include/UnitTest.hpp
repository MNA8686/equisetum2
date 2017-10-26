/**
* @file UnitTest.hpp
* @brief 単体テストサポートクラス(googletestとある程度の互換性あり)
* @author MNA
* @date 2017/07/07 新規作成
*/

#if !defined(_EQUNITTEST_H_)
#define _EQUNITTEST_H_

#include <string>
#include <vector>
#include "system/Logger.h"

namespace Equisetum2
{
	/**
	* 単体テスト定義クラス
	*/
	class TestDef
	{
	public:
		std::string test_case_name;		/// テストケース名
		std::string test_name;			/// テスト名
		bool passed = false;			/// テストパスフラグ
		virtual void TestBody() {}	/// 派生先のクラスで定義したこのメソッドをテスト項目として実装する
		virtual ~TestDef() {}
	};

	/**
	* 単体テスト制御
	*/
	class UnitTest
	{
	public:

		/**
		* @brief UnitTestオブジェクトを取得する
		* @return UnitTestオブジェクト
		*/
		static UnitTest* GetInstance();

		/**
		* @brief テストケースを登録する
		* @param TestDef* 単体テスト定義クラスのポインタ
		*/
		void Regist(TestDef* def);

		/**
		* @brief 登録した単体テストを実行する
		* @return 失敗したテストの数
		*/
		int Do();

		~UnitTest() {}

	private:

		std::vector<TestDef*> m_testQueue;		/// 登録された単体テスト
		static UnitTest* m_inst;				/// シングルトンオブジェクト
		UnitTest() {}
	};

#define TEST_CLASS_NAME(TEST_CASE_NAME, TEST_NAME) TEST_CASE_NAME##TEST_NAME

#define TEST(TEST_CASE_NAME, TEST_NAME) \
	class TEST_CLASS_NAME(TEST_CASE_NAME, TEST_NAME) : public TestDef \
	{ \
	public: \
		TEST_CLASS_NAME(TEST_CASE_NAME, TEST_NAME)() \
		{ \
			test_case_name = #TEST_CASE_NAME; \
			test_name = #TEST_NAME; \
			UnitTest::GetInstance()->Regist(this); \
		} \
		virtual void TestBody() override; \
	}; \
	static TEST_CLASS_NAME(TEST_CASE_NAME, TEST_NAME) TEST_CLASS_NAME(TEST_CASE_NAME, TEST_NAME)##inst; \
	void TEST_CLASS_NAME(TEST_CASE_NAME, TEST_NAME)::TestBody()

#define ASSERT_EQ(OP1, OP2) { \
	if (!((OP1) == (OP2))) \
	{ \
		auto str = String::Sprintf(u8"  %s(%d): error: ", __FILE__, __LINE__); \
		str += #OP1 u8" == " #OP2 u8" failed."; \
		Logger::OutputCritical(str.c_str()); \
		passed = false; \
		return; \
	} \
}

#define ASSERT_NE(OP1, OP2) {\
	if (!((OP1) != (OP2)))\
	{\
		auto str = String::Sprintf(u8"  %s(%d): error: ", __FILE__, __LINE__); \
		str += #OP1 u8" != " #OP2 u8" failed."; \
		Logger::OutputCritical(str.c_str()); \
		passed = false; \
		return; \
	}\
}

#define ASSERT_LT(OP1, OP2) {\
	if (!((OP1) < (OP2)))\
	{\
		auto str = String::Sprintf(u8"  %s(%d): error: ", __FILE__, __LINE__); \
		str += #OP1 u8" < " #OP2 u8" failed."; \
		Logger::OutputCritical(str.c_str()); \
		passed = false; \
		return; \
	}\
}

#define ASSERT_LE(OP1, OP2) {\
	if (!((OP1) <= (OP2)))\
	{\
		auto str = String::Sprintf(u8"  %s(%d): error: ", __FILE__, __LINE__); \
		str += #OP1 u8" <= " #OP2 u8" failed."; \
		Logger::OutputCritical(str.c_str()); \
		passed = false; \
		return; \
	}\
}

#define ASSERT_GT(OP1, OP2) {\
	if (!((OP1) > (OP2)))\
	{\
		auto str = String::Sprintf(u8"  %s(%d): error: ", __FILE__, __LINE__); \
		str += #OP1 u8" > " #OP2 u8" failed."; \
		Logger::OutputCritical(str.c_str()); \
		passed = false; \
		return; \
	}\
}

#define ASSERT_GE(OP1, OP2) {\
	if (!((OP1) >= (OP2)))\
	{\
		auto str = String::Sprintf(u8"  %s(%d): error: ", __FILE__, __LINE__); \
		str += #OP1 u8" >= " #OP2 u8" failed."; \
		Logger::OutputCritical(str.c_str()); \
		passed = false; \
		return; \
	}\
}

#define ASSERT_TRUE(OP1) {\
	if (!(OP1))\
	{\
		auto str = String::Sprintf(u8"  %s(%d): error: ", __FILE__, __LINE__); \
		str += #OP1 u8" == true failed."; \
		Logger::OutputCritical(str.c_str()); \
		passed = false; \
		return; \
	}\
}

#define ASSERT_FALSE(OP1) {\
	if ((OP1))\
	{\
		auto str = String::Sprintf(u8"  %s(%d): error: ", __FILE__, __LINE__); \
		str += #OP1 u8" == false failed."; \
		Logger::OutputCritical(str.c_str()); \
		passed = false; \
		return; \
	}\
}

}

#endif
