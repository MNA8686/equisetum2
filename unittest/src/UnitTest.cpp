/**
* @file UnitTest.cpp
* @brief 単体テストサポートクラス(googletestとある程度の互換性あり)
* @author MNA
* @date 2017/07/07 新規作成
*/

#include "UnitTest.hpp"

namespace Equisetum2
{
	// UnitTestのインスタンス削除クラス
	class UnitTestDeleter
	{
	public:
		~UnitTestDeleter()
		{
			auto p = UnitTest::GetInstance();
			if (p)
			{
				delete p;
				p = nullptr;
			}
		}
	};
	static UnitTestDeleter g_UnitTestDeleter;

	UnitTest* UnitTest::m_inst = nullptr;

	UnitTest* UnitTest::GetInstance()
	{
		if (!m_inst)
		{
			// グローバルクラスのインスタンス生成順序がどのようになっても、確実にインスタンスが存在することを保証する
			m_inst = new UnitTest;
		}

		return m_inst;
	}

	void UnitTest::Regist(TestDef* def)
	{
		// staticなクラスのポインタを格納するのでdeleteは不要
		m_testQueue.push_back(def);
	}

	int UnitTest::Do()
	{
		// 単体テスト実行
		for (auto&& elem : m_testQueue)
		{
			Logger::OutputInfo("[ RUN    ] %s %s", elem->test_case_name.c_str(), elem->test_name.c_str());

			elem->passed = true;
			elem->TestBody();	// テスト実行

			Logger::OutputInfo("[     %s ] %s %s", elem->passed ? "OK" : "NG", elem->test_case_name.c_str(), elem->test_name.c_str());
		}

		// エラー発生数算出
		int error = 0;
		for (auto&& elem : m_testQueue)
		{
			if (!elem->passed)
			{
				error++;
			}
		}

		// 結果出力
		if (error)
		{
			Logger::OutputInfo("[ FAILED ] %d error of %d tests.", error, m_testQueue.size());
		}
		else
		{
			Logger::OutputInfo("[ PASSED ] %d tests.", m_testQueue.size());
		}

		return error;
	}
}

