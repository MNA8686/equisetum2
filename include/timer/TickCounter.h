#if !defined(_EQTICKCOUNTER_H_)
#define _EQTICKCOUNTER_H_

#include "ITimer.h"
#include <memory>

namespace Equisetum2
{
	class TickCounter
	{
	public:

		// ファクトリー
		static std::shared_ptr<TickCounter> Create(std::shared_ptr<ITimer> timer);

		virtual void Reset();
		virtual uint32_t ElapsedTicks() const;

		// Pause
		// Resume

		static uint32_t ElapsedTicks(uint32_t past, uint32_t now);

	protected:

		explicit TickCounter();
		virtual ~TickCounter();
		virtual void SetTimer(std::shared_ptr<ITimer> timer);

	private:
		uint32_t m_PastTicks = 0;		// 現在のチック値
		std::shared_ptr<ITimer> m_Timer;

		TickCounter(const TickCounter&);				// コピーコンストラクタ封じ
		TickCounter& operator= (const TickCounter&);	// コピーコンストラクタ封じ
	};
}

#endif
