#if !defined(_EQTIMER_H_)
#define _EQTIMER_H_

#include <cstdint>

namespace Equisetum2
{
	// 時間を刻むだけのシンプルなタイマ
	// Update()を行うことで、内部で保持している時刻が更新される
	// 次のUpdate()を行うまで読み出せる時刻の値は変わらないことが保証されている
	class ITimer
	{
	public:
		virtual bool Update() = 0;	// 1フレームに一度タイマの値を更新する
		virtual uint32_t Ticks() const = 0;		// property: 保持されている時刻(ms)
	};
}

#endif
