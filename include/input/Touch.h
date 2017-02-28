#if !defined(_EQTOUCH_H_)
#define _EQTOUCH_H_

#include "input/IKeyState.h"
#include "input/Key.h"
#include "type/Point.h"
#include "type/Optional.h"
#include <vector>

namespace Equisetum2
{
	// FingerIndex �� FingerId �͈قȂ�
	// FingerIndex : �P���Ȕz��̃C���f�b�N�X 0�`�ő�T�|�[�g�t�B���K�[��
	// FingerId : �V�����w�����o�����x�Ɋ���U���鐔�l �ė��p�����\��������

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
