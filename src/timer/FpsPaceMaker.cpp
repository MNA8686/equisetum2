#include "timer/FpsPaceMaker.h"

#include "SDL.h"

namespace Equisetum2
{
	std::shared_ptr<FpsPaceMaker> FpsPaceMaker::Create(std::shared_ptr<TickCounter> tickCounter, int32_t fps)
	{
		class FpsPaceMakerDummy : public FpsPaceMaker
		{
			// このファクトリー以外でインスタンスを作らせないようにコンストラクタとデストラクタを
			// protected宣言しているが、そのままだとmake_sharedでエラーになるので一旦ダミークラスを挟む
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

		// 1フレームあたりどれぐらいの時間待つかを算出(固定小数点)
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
			// 必要時間経過している？
			if (fixedElapseTicks + m_fixdCarryTick >= m_fixdWaitTickPerFrame)
			{
				// 経過フレーム数を算出(フレームスキップで使用する)
				m_ElapsedFrame = (fixedElapseTicks + m_fixdCarryTick) / m_fixdWaitTickPerFrame;

				// 次のフレームを処理する時刻になった
				m_fixdCarryTick = (fixedElapseTicks + m_fixdCarryTick) % m_fixdWaitTickPerFrame;
				m_TickCounter->Reset();
			}
			else
			{
				// まだ1フレーム分の時間が経過していない
				// 残りの時間を算出
				remain = (m_fixdWaitTickPerFrame - (fixedElapseTicks + m_fixdCarryTick)) >> m_FractionalPart;
				// 右シフトした結果、残り時間が0になる可能性があるので補正
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
