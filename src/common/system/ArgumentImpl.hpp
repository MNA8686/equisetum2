/**
* @file ArgumentImpl.hpp
* @brief 引数取得実装クラス
* @author MNA
* @date 2017/10/28 新規作成
*/
#if !defined(_EQARGUMENTIMPL_H_)
#define _EQARGUMENTIMPL_H_

#include "system/Singleton.h"
#include "type/String.h"
#include "util/Step.hpp"

namespace Equisetum2
{
	class ArgumentImpl final
	{
	public:
		friend Singleton<ArgumentImpl>;	// シングルトンからインスタンスを作成してもらえるようにする

		ArgumentImpl() = default;
		~ArgumentImpl() = default;

		void Set(int argc, char *argv[])
		{
			if (argc > 1)
			{
				for (auto& i : Step(1, argc - 1))
				{
					m_vArgv.emplace_back(argv[i.x]);
				}
			}
		}

		const std::vector<String>& Get()
		{
			return m_vArgv;
		}

	private:

		std::vector<String> m_vArgv;
	};
}

#endif
