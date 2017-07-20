#include "timer/SystemTimerCompat.h"

#include "timer/SystemTimerCompatImpl.inl"

namespace Equisetum2
{
	SystemTimerCompat::SystemTimerCompat()
	{
		m_Impl = std::make_shared<SystemTimerCompat::Impl>();
	}

	uint32_t SystemTimerCompat::Ticks() const
	{
		return m_Impl->Ticks();
	}
}
