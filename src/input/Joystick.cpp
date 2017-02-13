#include "input/Joystick.h"
#include "system/Logger.h"
#include "input/JoystickInternal.h"

namespace Equisetum2
{
	std::shared_ptr<Joystick> Joystick::CreateByIndex(int index)
	{
		if (index < 0)
		{
			return nullptr;
		}

		class JoystickDummy : public Joystick
		{
		public:
			JoystickDummy(int index) : Joystick(index){}
		};
		return std::make_shared<JoystickDummy>(index);
	}

	Joystick::Joystick(int index)
	{
		m_Index = index;
	}

	bool Joystick::IsConnected() const
	{
		return Singleton<JoystickInternal>::GetInstance()->IsConnected(m_Index);
	}

	int16_t Joystick::Axis(int num) const
	{
		return Singleton<JoystickInternal>::GetInstance()->Axis(m_Index, num);
	}

	HatState Joystick::Hat(int num) const
	{
		return Singleton<JoystickInternal>::GetInstance()->Hat(m_Index, num);
	}

	const Key Joystick::Button(int num) const
	{
		int index = m_Index;

		const Key joyButton([index, num]()->stKeyState
		{
			return Singleton<JoystickInternal>::GetInstance()->Button(index, num);
		});

		return joyButton;
	}

	int Joystick::NumAxes() const
	{
		return Singleton<JoystickInternal>::GetInstance()->NumAxes(m_Index);
	}

	int Joystick::NumButtons() const
	{
		return Singleton<JoystickInternal>::GetInstance()->NumButtons(m_Index);
	}

	int Joystick::NumHats() const
	{
		return Singleton<JoystickInternal>::GetInstance()->NumHats(m_Index);
	}
	
	const std::string Joystick::Name() const
	{
		return Singleton<JoystickInternal>::GetInstance()->Name(m_Index);
	}

	void Joystick::SetEventListener(const stJoystickEvent& listener)
	{
		// TODO 挿抜イベントなどはここで登録する
		//Singleton<JoystickInternal>::GetInstance()->SetEventListener(this, listener);
		// deleteされるようにすること
	}

	int Joystick::NumJoysticks()
	{
		return Singleton<JoystickInternal>::GetInstance()->NumJoysticks();
	}
}

