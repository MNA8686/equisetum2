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
	* �R�}���h����
	*/
	class Command
	{
	public:

		/**
		* @brief Command�I�u�W�F�N�g���擾����
		* @return Command�I�u�W�F�N�g
		*/
		static Command* GetInstance();

		/**
		* @brief �R�}���h��o�^����
		* @param stCommand* �P�̃e�X�g��`�N���X�̃|�C���^
		*/
		void Regist(const stCommand* def);

		/**
		* @brief �K�؂ȃR�}���h�����s����
		* @return �R�}���h�̖߂�l
		*/
		int Do();

		~Command() {}

	private:

		std::vector<const stCommand*> m_vCommand;		/// �o�^���ꂽ�R�}���h
		static Command* m_inst;				/// �V���O���g���I�u�W�F�N�g
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
