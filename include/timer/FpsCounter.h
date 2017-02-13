#if !defined(_EQFPSCOUNTER_H_)
#define _EQFPSCOUNTER_H_

#include "TickCounter.h"
#include <memory>

namespace Equisetum2
{
	class FpsCounter
	{
	public:

		// ファクトリー
		// 内部でカウンターのリセットなどを行うため、ここにはこのクラス専用のカウンターを設定すること
		static std::shared_ptr<FpsCounter> Create(std::shared_ptr<TickCounter> tickCounter);

		virtual void Reset();
		virtual void Increase();
		virtual uint32_t Fps() const;

	protected:

		explicit FpsCounter();
		virtual ~FpsCounter();
		virtual void SetTickCounter(std::shared_ptr<TickCounter> tickCounter);

	private:

		uint32_t m_NowFps = 0;		// カウント中のFPS
		uint32_t m_RecentFps = 0;		// 1秒前のFPS
		std::shared_ptr<TickCounter> m_TickCounter;

		FpsCounter(const FpsCounter&);				// コピーコンストラクタ封じ
		FpsCounter& operator= (const FpsCounter&);	// コピーコンストラクタ封じ
	};
}

#endif
