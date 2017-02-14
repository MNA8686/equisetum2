#include "timer/FpsCounter.h"

namespace Equisetum2
{
	std::shared_ptr<FpsCounter> FpsCounter::Create(std::shared_ptr<TickCounter> tickCounter)
	{
		class FpsCounterDummy : public FpsCounter
		{
			// ���̃t�@�N�g���[�ȊO�ŃC���X�^���X����点�Ȃ��悤�ɃR���X�g���N�^�ƃf�X�g���N�^��
			// protected�錾���Ă��邪�A���̂܂܂���make_shared�ŃG���[�ɂȂ�̂ň�U�_�~�[�N���X������
		};

		std::shared_ptr<FpsCounter> inst;

		if (tickCounter)
		{
			auto inst_ = std::make_shared<FpsCounterDummy>();
			if (inst_)
			{
				inst_->SetTickCounter(tickCounter);
				inst = inst_;
			}
		}
		return inst;
	}

	FpsCounter::FpsCounter()
	{
	}

	FpsCounter::~FpsCounter()
	{
	}

	void FpsCounter::Reset()
	{
		m_TickCounter->Reset();
		m_NowFps = 0;
	}

	uint32_t FpsCounter::Fps() const
	{
		return m_RecentFps;	// 1�b�O��FPS��Ԃ�
	}

	void FpsCounter::Increase()
	{
		m_NowFps++;

		// 1�b�o�߂����H
		if (m_TickCounter->ElapsedTicks() >= 1000)
		{
			m_TickCounter->Reset();	// �[���͋C�ɂ������Z�b�g
			m_RecentFps = m_NowFps;
			m_NowFps = 0;
		}
	}

	void FpsCounter::SetTickCounter(std::shared_ptr<TickCounter> tickCounter)
	{
		m_TickCounter = tickCounter;
	}
}
