#include "input/Joystick.h"
#include "system/Logger.h"
#include "input/JoystickCompat.h"

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

	Joystick::~Joystick()
	{
		// ÉäÉXÉiÅ[çÌèú
		Singleton<JoystickCompat>::GetInstance()->SetEventListener(m_Index, {}, this);
	}

	Joystick::Joystick(int index)
	{
		m_Index = index;
	}

	bool Joystick::IsConnected() const
	{
		return Singleton<JoystickCompat>::GetInstance()->IsConnected(m_Index);
	}

	int16_t Joystick::Axis(int num) const
	{
		return Singleton<JoystickCompat>::GetInstance()->Axis(m_Index, num);
	}

	const Key Joystick::HatUp(int num) const
	{
		int index = m_Index;

		const Key joyHat([index, num]()->stKeyState
		{
			return Singleton<JoystickCompat>::GetInstance()->HatUp(index, num);
		});

		return joyHat;
	}

	const Key Joystick::HatDown(int num) const
	{
		int index = m_Index;

		const Key joyHat([index, num]()->stKeyState
		{
			return Singleton<JoystickCompat>::GetInstance()->HatDown(index, num);
		});

		return joyHat;
	}

	const Key Joystick::HatLeft(int num) const
	{
		int index = m_Index;

		const Key joyHat([index, num]()->stKeyState
		{
			return Singleton<JoystickCompat>::GetInstance()->HatLeft(index, num);
		});

		return joyHat;
	}

	const Key Joystick::HatRight(int num) const
	{
		int index = m_Index;

		const Key joyHat([index, num]()->stKeyState
		{
			return Singleton<JoystickCompat>::GetInstance()->HatRight(index, num);
		});

		return joyHat;
	}

	const Key Joystick::Button(int num) const
	{
		int index = m_Index;

		const Key joyButton([index, num]()->stKeyState
		{
			return Singleton<JoystickCompat>::GetInstance()->Button(index, num);
		});

		return joyButton;
	}

	int Joystick::NumAxes() const
	{
		return Singleton<JoystickCompat>::GetInstance()->NumAxes(m_Index);
	}

	int Joystick::NumButtons() const
	{
		return Singleton<JoystickCompat>::GetInstance()->NumButtons(m_Index);
	}

	int Joystick::NumHats() const
	{
		return Singleton<JoystickCompat>::GetInstance()->NumHats(m_Index);
	}
	
	const String Joystick::Name() const
	{
		return Singleton<JoystickCompat>::GetInstance()->Name(m_Index);
	}

	int Joystick::Index()
	{
		return m_Index;
	}

	void Joystick::SetEventListener(const stJoystickEvent& listener)
	{
		Singleton<JoystickCompat>::GetInstance()->SetEventListener(m_Index, listener, this);
	}

	int Joystick::NumJoysticks()
	{
		return Singleton<JoystickCompat>::GetInstance()->NumJoysticks();
	}
}

