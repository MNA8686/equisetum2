#pragma once

#include "Equisetum2.h"

namespace Equisetum2
{
	typedef struct
	{
		String command;
		String usage;
		std::function<int(void)> func;
	}stCommand;

	/**
	* コマンド制御
	*/
	class Command
	{
	public:

		/**
		* @brief Commandオブジェクトを取得する
		* @return Commandオブジェクト
		*/
		static Command* GetInstance();

		/**
		* @brief コマンドを登録する
		* @param stCommand* 単体テスト定義クラスのポインタ
		*/
		void Regist(const stCommand* def);

		/**
		* @brief 適切なコマンドを実行する
		* @return コマンドの戻り値
		*/
		int Do();

		~Command() {}

	private:

		std::vector<const stCommand*> m_vCommand;		/// 登録されたコマンド
		static Command* m_inst;				/// シングルトンオブジェクト
		Command() {};

		void Usage() const;
	};

#define COMMAND_DEF(name, usage) \
	static int Do(); \
	static const stCommand cmd = \
	{ \
		(#name), \
		"eqtools " + String(#name) + " " + (usage), \
		Do \
	}; \
\
	namespace Command_##name \
	{ \
		class Regist \
		{ \
		public: \
			Regist() \
			{ \
				Command::GetInstance()->Regist(&cmd); \
			} \
		}; \
		static Regist regist; \
	} \
\
	static int Do() \

}
