#if !defined(_EQSYSTEMTTIMER_H_)
#define _EQSYSTEMTTIMER_H_

#include "ITimer.h"
#include <memory>

namespace Equisetum2
{
	class SystemTimer : public ITimer
	{
	public:

		// �t�@�N�g���[
		static std::shared_ptr<SystemTimer> Create();

		virtual bool Update() override;
		virtual uint32_t Ticks() const override;

	protected:

		explicit SystemTimer();
		virtual ~SystemTimer();

	private:
		uint32_t m_Ticks = 0;		// ���݂̃`�b�N�l

		SystemTimer(const SystemTimer&) = delete;				// �R�s�[�R���X�g���N�^����
		SystemTimer& operator= (const SystemTimer&) = delete;	// �R�s�[�R���X�g���N�^����
	};
}

#endif
