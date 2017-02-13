#include "timer/SystemTimer.h"
#include "timer/SystemTimerInternal.h"

namespace Equisetum2
{
	std::shared_ptr<SystemTimer> SystemTimer::Create()
	{
		class SystemTimerDummy : public SystemTimer
		{
			// ���̃t�@�N�g���[�ȊO�ŃC���X�^���X����点�Ȃ��悤�ɃR���X�g���N�^�ƃf�X�g���N�^��
			// protected�錾���Ă��邪�A���̂܂܂���make_shared�ŃG���[�ɂȂ�̂ň�U�_�~�[�N���X������
		};

		std::shared_ptr<SystemTimer> inst;
		auto inst_ = std::make_shared<SystemTimerDummy>();
		if (inst_)
		{
			inst = inst_;
		}
		return inst;
	}

	SystemTimer::SystemTimer()
	{
	}

	SystemTimer::~SystemTimer()
	{
	}

	bool SystemTimer::Update()
	{
		m_Ticks = Singleton<SystemTimerInternal>::GetInstance()->Ticks();
		return true;
	}

	uint32_t SystemTimer::Ticks() const
	{
		return m_Ticks;
	}
}


