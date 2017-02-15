#if !defined(_EQSYSTEMTIMERCOMPAT_H_)
#define _EQSYSTEMTIMERCOMPAT_H_

#include "system/Singleton.h"
#include <memory>

namespace Equisetum2
{
	class SystemTimerCompat final
	{
	public:
		friend Singleton<SystemTimerCompat>;	// �V���O���g������C���X�^���X���쐬���Ă��炦��悤�ɂ���

		uint32_t Ticks() const;

	private:

		class Impl;
		std::shared_ptr<Impl> m_Impl;
		SystemTimerCompat();	// �C���X�^���X�쐬����
	};
}

#endif
