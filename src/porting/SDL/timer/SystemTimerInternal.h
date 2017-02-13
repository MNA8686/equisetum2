#if !defined(_EQSYSTEMTIMERINTERNAL_H_)
#define _EQSYSTEMTIMERINTERNAL_H_

#include "system/Singleton.h"
#include <memory>

namespace Equisetum2
{
	class SystemTimerInternal final
	{
	public:
		friend Singleton<SystemTimerInternal>;	// �V���O���g������C���X�^���X���쐬���Ă��炦��悤�ɂ���

		uint32_t Ticks() const;

	private:

		class Impl;
		std::shared_ptr<Impl> m_Impl;
		SystemTimerInternal();	// �C���X�^���X�쐬����
	};
}

#endif
