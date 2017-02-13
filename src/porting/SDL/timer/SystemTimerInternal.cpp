#include "timer/SystemTimerInternal.h"

#include "timer/SystemTimerInternalImpl.inl"

namespace Equisetum2
{
	SystemTimerInternal::SystemTimerInternal()
	{
		m_Impl = std::make_shared<SystemTimerInternal::Impl>();
	}

	uint32_t SystemTimerInternal::Ticks() const
	{
		return m_Impl->Ticks();
	}
}
