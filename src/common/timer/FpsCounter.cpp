#include "timer/FpsCounter.h"

namespace Equisetum2
{
	std::shared_ptr<FpsCounter> FpsCounter::Create(std::shared_ptr<TickCounter> tickCounter)
	{
		class FpsCounterDummy : public FpsCounter
		{
			// このファクトリー以外でインスタンスを作らせないようにコンストラクタとデストラクタを
			// protected宣言しているが、そのままだとmake_sharedでエラーになるので一旦ダミークラスを挟む
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
		return m_RecentFps;	// 1秒前のFPSを返す
	}

	void FpsCounter::Increase()
	{
		m_NowFps++;

		// 1秒経過した？
		if (m_TickCounter->ElapsedTicks() >= 1000)
		{
			m_TickCounter->Reset();	// 端数は気にせずリセット
			m_RecentFps = m_NowFps;
			m_NowFps = 0;
		}
	}

	void FpsCounter::SetTickCounter(std::shared_ptr<TickCounter> tickCounter)
	{
		m_TickCounter = tickCounter;
	}
}
