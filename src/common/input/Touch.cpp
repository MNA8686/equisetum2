#include "input/Touch.h"
#include "system/Logger.h"
#include "input/TouchCompat.h"

namespace Equisetum2
{
	const PointF TouchState::Pos() const
	{
		return m_Pos;
	}

	const PointF TouchState::Delta() const
	{
		return m_Delta;
	}

	int64_t TouchState::FingerId() const
	{
		return m_FingerId;
	}

	const String TouchState::Name() const
	{
		return m_KeyState.name;
	}

	bool TouchState::IsDown() const
	{
		return m_KeyState.down;
	}

	bool TouchState::IsUp() const
	{
		return m_KeyState.up;
	}

	bool TouchState::IsPress() const
	{
		return m_KeyState.press;
	}

	uint32_t TouchState::TimeStamp() const
	{
		return m_KeyState.timeStamp;
	}

	uint32_t TouchState::PressedDuration() const
	{
		return m_KeyState.pressedDuration;
	}


	const Optional<TouchState> Touch::GetStateByFingerIndex(int fingerIndex)
	{
		class TouchStateWithSetter : public TouchState
		{
		public:
			TouchStateWithSetter(int fingerIndex)
			{
				m_Pos = Singleton<TouchCompat>::GetInstance()->Pos(fingerIndex);
				m_Delta = Singleton<TouchCompat>::GetInstance()->Delta(fingerIndex);
				m_FingerId = Singleton<TouchCompat>::GetInstance()->FingerId(fingerIndex);
				m_KeyState = Singleton<TouchCompat>::GetInstance()->Button(fingerIndex);
			}
		};

		Optional<TouchState> state;
		if (fingerIndex >= 0 && fingerIndex < NumSupportedFinger())
		{
			state = TouchStateWithSetter(fingerIndex);
		}
		return state;
	}

	const std::vector<TouchState> Touch::GetTouches()
	{
		std::vector<TouchState> vState;
		for (int i = 0; i < NumSupportedFinger(); i++)
		{
			if (auto state = GetStateByFingerIndex(i))
			{
				// 押されている指と離された瞬間を有効なものとして処理する
				if ((*state).IsPress() || (*state).IsUp())
				{
					vState.push_back((*state));
				}
			}
		}

		return vState;
	}

	int Touch::NumSupportedFinger()
	{
		return Singleton<TouchCompat>::GetInstance()->NumSupportedFinger();
	}
}

