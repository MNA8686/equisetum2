#if !defined(_EQSYSTEMTTIMER_H_)
#define _EQSYSTEMTTIMER_H_

#include "ITimer.h"
#include <memory>

namespace Equisetum2
{
	class SystemTimer : public ITimer
	{
	public:

		// ファクトリー
		static std::shared_ptr<SystemTimer> Create();

		virtual bool Update() override;
		virtual uint32_t Ticks() const override;

	protected:

		explicit SystemTimer();
		virtual ~SystemTimer();

	private:
		uint32_t m_Ticks = 0;		// 現在のチック値

		SystemTimer(const SystemTimer&) = delete;				// コピーコンストラクタ封じ
		SystemTimer& operator= (const SystemTimer&) = delete;	// コピーコンストラクタ封じ
	};
}

#endif
