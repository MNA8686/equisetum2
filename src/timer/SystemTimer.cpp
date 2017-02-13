#include "timer/SystemTimer.h"
#include "timer/SystemTimerInternal.h"

namespace Equisetum2
{
	std::shared_ptr<SystemTimer> SystemTimer::Create()
	{
		class SystemTimerDummy : public SystemTimer
		{
			// このファクトリー以外でインスタンスを作らせないようにコンストラクタとデストラクタを
			// protected宣言しているが、そのままだとmake_sharedでエラーになるので一旦ダミークラスを挟む
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


