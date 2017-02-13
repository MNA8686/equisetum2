#if !defined(_EQFPSPACEMAKER_H_)
#define _EQFPSPACEMAKER_H_

#include "TickCounter.h"
#include <memory>

namespace Equisetum2
{
	class FpsPaceMaker
	{
	public:

		// ファクトリー
		// 内部でカウンターのリセットなどを行うため、ここにはこのクラス専用のカウンターを設定すること
		static std::shared_ptr<FpsPaceMaker> Create(std::shared_ptr<TickCounter> tickCounter, int32_t fps);

		virtual void Reset();
		virtual void SetTargetFps(uint32_t fps);
		virtual uint32_t TargetFps() const;
		virtual uint32_t RemainTicks();	// 次のフレーム開始までの残り時間を取得する(ms)
		virtual uint32_t ElapsedFrames();	// 何フレーム経過したか(RemainTicksのあとで呼ぶこと)

	protected:

		explicit FpsPaceMaker();
		virtual ~FpsPaceMaker();
		virtual void SetTickCounter(std::shared_ptr<TickCounter> tickCounter);

	private:

		uint32_t m_TargetFps = 0;		// 現在設定されているFPS
		uint32_t m_fixdWaitTickPerFrame = 0;	// 1フレームあたりの待ち時間(ms)
		uint32_t m_fixdCarryTick = 0;	// 繰り越された時間
		uint32_t m_ElapsedFrame = 0;	// 経過フレーム数
		std::shared_ptr<TickCounter> m_TickCounter;

		FpsPaceMaker(const FpsPaceMaker&);				// コピーコンストラクタ封じ
		FpsPaceMaker& operator= (const FpsPaceMaker&);	// コピーコンストラクタ封じ

		const int32_t m_FractionalPart = 16;	// 固定小数点の小数部分の桁数
	};
}

#endif
