/**
* @file Command.cpp
* @brief 単体テストサポートクラス(googletestとある程度の互換性あり)
* @author MNA
* @date 2017/07/07 新規作成
*/

#include "Command.hpp"

namespace Equisetum2
{
	// Commandのインスタンス削除クラス
	class CommandDeleter
	{
	public:
		~CommandDeleter()
		{
			auto p = Command::GetInstance();
			if (p)
			{
				delete p;
				p = nullptr;
			}
		}
	};
	static CommandDeleter g_CommandDeleter;

	Command* Command::m_inst = nullptr;

	Command* Command::GetInstance()
	{
		if (!m_inst)
		{
			// グローバルクラスのインスタンス生成順序がどのようになっても、確実にインスタンスが存在することを保証する
			m_inst = new Command;
		}

		return m_inst;
	}

	void Command::Regist(const stCommand* def)
	{
		// staticなクラスのポインタを格納するのでdeleteは不要
		m_vCommand.push_back(def);
	}

	void Command::Usage() const
	{
		printf("usage:\n");
		for (auto& cmd : m_vCommand)
		{
			printf(cmd->usage.c_str());
			printf("\n");
		}
	}

	int Command::Do()
	{
		auto it = Argument().begin();

		if (Argument().size() == 0 ||
			*it == "?" ||
			*it == "-help")
		{
			Usage();
			return 0;
		}

		for (auto& cmd : m_vCommand)
		{
			if (cmd->command == *it)
			{
				return cmd->func();
			}
		}

		printf("unknown command <%s>\n", (*it).c_str());
		return -1;
	}
}

