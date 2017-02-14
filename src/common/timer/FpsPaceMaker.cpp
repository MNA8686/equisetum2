#include "timer/FpsPaceMaker.h"

#include "SDL.h"

namespace Equisetum2
{
	std::shared_ptr<FpsPaceMaker> FpsPaceMaker::Create(std::shared_ptr<TickCounter> tickCounter, int32_t fps)
	{
		class FpsPaceMakerDummy : public FpsPaceMaker
		{
			// ���̃t�@�N�g���[�ȊO�ŃC���X�^���X����点�Ȃ��悤�ɃR���X�g���N�^�ƃf�X�g���N�^��
			// protected�錾���Ă��邪�A���̂܂܂���make_shared�ŃG���[�ɂȂ�̂ň�U�_�~�[�N���X������
		};

		std::shared_ptr<FpsPaceMaker> inst;

		if (tickCounter)
		{
			auto inst_ = std::make_shared<FpsPaceMakerDummy>();
			if (inst_)
			{
				inst_->SetTickCounter(tickCounter);
				inst_->SetTargetFps(fps);
				inst = inst_;
			}
		}
		return inst;
	}

	FpsPaceMaker::FpsPaceMaker()
	{
	}

	FpsPaceMaker::~FpsPaceMaker()
	{
	}

	void FpsPaceMaker::Reset()
	{
		m_TickCounter->Reset();
		m_fixdCarryTick = 0;
	}

	void FpsPaceMaker::SetTargetFps(uint32_t fps)
	{
		m_TargetFps = fps;

		// 1�t���[��������ǂꂮ�炢�̎��ԑ҂����Z�o(�Œ菬���_)
		if (m_TargetFps == 0)
		{
			m_fixdWaitTickPerFrame = 0;
		}
		else
		{
			m_fixdWaitTickPerFrame = (1000 << m_FractionalPart) / m_TargetFps;
		}

		Reset();
	}

	uint32_t FpsPaceMaker::TargetFps() const
	{
		return m_TargetFps;
	}

	uint32_t FpsPaceMaker::RemainTicks()
	{
		uint32_t fixedElapseTicks = m_TickCounter->ElapsedTicks() << m_FractionalPart;
		uint32_t remain = 0;

		if (m_fixdWaitTickPerFrame > 0)
		{
			// �K�v���Ԍo�߂��Ă���H
			if (fixedElapseTicks + m_fixdCarryTick >= m_fixdWaitTickPerFrame)
			{
				// �o�߃t���[�������Z�o(�t���[���X�L�b�v�Ŏg�p����)
				m_ElapsedFrame = (fixedElapseTicks + m_fixdCarryTick) / m_fixdWaitTickPerFrame;

				// ���̃t���[�����������鎞���ɂȂ���
				m_fixdCarryTick = (fixedElapseTicks + m_fixdCarryTick) % m_fixdWaitTickPerFrame;
				m_TickCounter->Reset();
			}
			else
			{
				// �܂�1�t���[�����̎��Ԃ��o�߂��Ă��Ȃ�
				// �c��̎��Ԃ��Z�o
				remain = (m_fixdWaitTickPerFrame - (fixedElapseTicks + m_fixdCarryTick)) >> m_FractionalPart;
				// �E�V�t�g�������ʁA�c�莞�Ԃ�0�ɂȂ�\��������̂ŕ␳
				if (remain == 0)
				{
					remain = 1;
				}
			}
		}

		return remain;
	}

	uint32_t FpsPaceMaker::ElapsedFrames()
	{
		return m_ElapsedFrame;
	}

	void FpsPaceMaker::SetTickCounter(std::shared_ptr<TickCounter> tickCounter)
	{
		m_TickCounter = tickCounter;
	}
}
