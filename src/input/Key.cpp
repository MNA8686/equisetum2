#include "input/Key.h"
#include "input/KeyboardCompat.h"
#include "timer/TickCounter.h"
#include "timer/SystemTimerCompat.h"
#include "system/Logger.h"

#include <unordered_map>

namespace Equisetum2
{
	Key::Key()
	{
		m_Listener = []()->stKeyState{ return{"null key"}; };
	}

	Key::Key(const std::function<stKeyState()>& listener)
	{
		if (listener != nullptr)
		{
			m_Listener = listener;
		}
	}

	bool Key::IsDown() const
	{
		return m_Listener().down;
	}

	bool Key::IsUp() const
	{
		return m_Listener().up;
	}

	bool Key::IsPress() const
	{
		return m_Listener().press;
	}

	uint32_t Key::TimeStamp() const
	{
		return m_Listener().timeStamp;
	}

	uint32_t Key::PressedDuration() const
	{
		return m_Listener().pressedDuration;
	}

	const std::string Key::Name() const
	{
		return m_Listener().name;
	}

	const std::function<stKeyState()> KB::CreateKeyboardFunc(KeyCode keyCode)
	{
		return [keyCode]()->stKeyState
		{
			return Singleton<KeyboardCompat>::GetInstance()->GetKeyState(keyCode);
		};
	}

	// タイムスタンプ管理構造体
	// (キー押下時に押された時間を保持したいが、IsDown()はconstなのでやむなくクラスの外部に持つこととする)
	typedef struct
	{
		uint32_t down;
		uint32_t last;
	}stCombinationKeyTimeStamp;
	static std::unordered_map<const CombinationKey*, stCombinationKeyTimeStamp> g_mapCombinationKeyTimeStamp;

	void CombinationKey::RegisterTimeStamp()
	{
		g_mapCombinationKeyTimeStamp[this] = {};
	}

	CombinationKey::~CombinationKey()
	{
		g_mapCombinationKeyTimeStamp.erase(this);
	}

	void CombinationKey::Append(const Key& key)
	{
		m_vKey.push_back(key);
	}

	const std::string CombinationKey::Name() const
	{
		std::string name = "(";
		int i = 0;

		for (auto& key : m_vKey)
		{
			if (i > 0)
			{
				name += " | ";
			}

			if (key.Name().empty())
			{
				name += "(empty)";
			}
			else
			{
				name += key.Name();
			}

			i++;
		}

		name += ")";

		return name;
	}

	void CombinationKey::Append(const CombinationKey& combKey)
	{
		for (auto& key : combKey.m_vKey)
		{
			Append(key);
		}
	}

	bool CombinationKey::IsDown() const
	{
		int numDown = 0;
		int numPress = 0;
		bool down = false;
		uint32_t duration = 0;
		uint32_t timeStamp = 0;

		for (auto& key : m_vKey)
		{
			if (key.IsDown())
			{
				numDown++;

				// 押下判定開始時点で最も長く押されているキーのタイムスタンプを採用する
				if (duration <= key.PressedDuration())
				{
					duration = key.PressedDuration();
					timeStamp = key.TimeStamp() - duration;
				}
			}
			if (key.IsPress())
			{
				numPress++;
			}
		}

		if (numDown > 0 &&
			numDown == numPress)
		{
			down = true;

			// 押された時間を記録
			g_mapCombinationKeyTimeStamp[this].down = timeStamp;
		}

		return down;
	}

	bool CombinationKey::IsUp() const
	{
		bool up = false;
		auto press = IsPress();
		if (!press)
		{
			for (auto& key : m_vKey)
			{
				if (key.IsUp())
				{
					up = true;

					// 離された時間を記録
					g_mapCombinationKeyTimeStamp[this].last = Singleton<SystemTimerCompat>::GetInstance()->Ticks();
					break;
				}
			}
		}

		return up;
	}

	bool CombinationKey::IsPress() const
	{
		bool press = false;

		for (auto& key : m_vKey)
		{
			if (key.IsPress())
			{
				press = true;
				break;
			}
		}

		return press;
	}

	uint32_t CombinationKey::TimeStamp() const
	{
		return g_mapCombinationKeyTimeStamp[this].down;
	}

	uint32_t CombinationKey::PressedDuration() const
	{
		uint32_t duration = 0;
		const auto& ref = g_mapCombinationKeyTimeStamp[this];

		if (IsPress())
		{
			// 押されている間は押下開始時と現在時刻の差を求める
			duration = TickCounter::ElapsedTicks(ref.down, Singleton<SystemTimerCompat>::GetInstance()->Ticks());
		}
		else
		{
			// 離されていたら押下開始時と離された時間の差を求める
			duration = TickCounter::ElapsedTicks(ref.down, ref.last);
		}

		return duration;
	}
}
