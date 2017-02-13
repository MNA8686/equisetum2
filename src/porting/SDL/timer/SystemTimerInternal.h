#if !defined(_EQSYSTEMTIMERINTERNAL_H_)
#define _EQSYSTEMTIMERINTERNAL_H_

#include "system/Singleton.h"
#include <memory>

namespace Equisetum2
{
	class SystemTimerInternal final
	{
	public:
		friend Singleton<SystemTimerInternal>;	// シングルトンからインスタンスを作成してもらえるようにする

		uint32_t Ticks() const;

	private:

		class Impl;
		std::shared_ptr<Impl> m_Impl;
		SystemTimerInternal();	// インスタンス作成封じ
	};
}

#endif
