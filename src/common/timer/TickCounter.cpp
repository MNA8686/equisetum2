#include "timer/TickCounter.h"

namespace Equisetum2
{
	std::shared_ptr<TickCounter> TickCounter::Create(std::shared_ptr<ITimer> timer)
	{
		class TickCounterDummy : public TickCounter
		{
			// このファクトリー以外でインスタンスを作らせないようにコンストラクタとデストラクタを
			// protected宣言しているが、そのままだとmake_sharedでエラーになるので一旦ダミークラスを挟む
		};

		std::shared_ptr<TickCounter> inst;

		if (timer)
		{
			auto inst_ = std::make_shared<TickCounterDummy>();
			if (inst_)
			{
				inst_->SetTimer(timer);
				inst = inst_;
			}
		}
		return inst;
	}

	TickCounter::TickCounter()
	{
	}

	TickCounter::~TickCounter()
	{
	}

	void TickCounter::Reset()
	{
		m_PastTicks = m_Timer->Ticks();
	}

	uint32_t TickCounter::ElapsedTicks() const
	{
		return ElapsedTicks(m_PastTicks, m_Timer->Ticks());
	}

	uint32_t TickCounter::ElapsedTicks(uint32_t past, uint32_t now)
	{
		uint32_t elapse = 0;

		if (now >= past)
		{
			elapse = now - past;
		}
		else
		{
			elapse = now + (~0 - past + 1);
		}

		return elapse;

	}

	void TickCounter::SetTimer(std::shared_ptr<ITimer> timer)
	{
		m_Timer = timer;
	}

}


