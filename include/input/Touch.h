#if !defined(_EQTOUCH_H_)
#define _EQTOUCH_H_

#include "input/IKeyState.h"
#include "input/Key.h"
#include "type/Point.h"
#include "type/Optional.h"
#include <vector>

namespace Equisetum2
{
	// FingerIndex と FingerId は異なる
	// FingerIndex : 単純な配列のインデックス 0～最大サポートフィンガー数
	// FingerId : 新しい指が検出される度に割り振られる数値 再利用される可能性がある

	class TouchState : public IKeyState
	{
	public:
		const Point<float> Pos() const;
		const Point<float> Delta() const;
		int64_t FingerId() const;

		const std::string Name() const override;
		bool IsDown() const override;
		bool IsUp() const override;
		bool IsPress() const override;
		uint32_t TimeStamp() const override;
		uint32_t PressedDuration() const override;

	protected:

		Point<float> m_Pos;
		Point<float> m_Delta;
		int64_t m_FingerId = 0;
		stKeyState m_KeyState = {};
	};

	class Touch
	{
	public:
		static const Optional<TouchState> GetStateByFingerIndex(int fingerIndex);
		static int NumSupportedFinger();
		static const std::vector<TouchState> GetTouches();
	};
}

#endif
