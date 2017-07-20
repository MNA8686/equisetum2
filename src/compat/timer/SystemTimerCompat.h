#if !defined(_EQSYSTEMTIMERCOMPAT_H_)
#define _EQSYSTEMTIMERCOMPAT_H_

#include "system/Singleton.h"
#include <memory>

namespace Equisetum2
{
	class SystemTimerCompat final
	{
	public:
		friend Singleton<SystemTimerCompat>;	// シングルトンからインスタンスを作成してもらえるようにする

		uint32_t Ticks() const;

	private:

		class Impl;
		std::shared_ptr<Impl> m_Impl;
		SystemTimerCompat();	// インスタンス作成封じ
	};
}

#endif
