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

	Joystick::~Joystick()
	{
		// ÉäÉXÉiÅ[çÌèú
		Singleton<JoystickInternal>::GetInstance()->SetEventListener(m_Index, {}, this);
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

	const Key Joystick::HatUp(int num) const
	{
		int index = m_Index;

		const Key joyHat([index, num]()->stKeyState
		{
			return Singleton<JoystickInternal>::GetInstance()->HatUp(index, num);
		});

		return joyHat;
	}

	const Key Joystick::HatDown(int num) const
	{
		int index = m_Index;

		const Key joyHat([index, num]()->stKeyState
		{
			return Singleton<JoystickInternal>::GetInstance()->HatDown(index, num);
		});

		return joyHat;
	}

	const Key Joystick::HatLeft(int num) const
	{
		int index = m_Index;

		const Key joyHat([index, num]()->stKeyState
		{
			return Singleton<JoystickInternal>::GetInstance()->HatLeft(index, num);
		});

		return joyHat;
	}

	const Key Joystick::HatRight(int num) const
	{
		int index = m_Index;

		const Key joyHat([index, num]()->stKeyState
		{
			return Singleton<JoystickInternal>::GetInstance()->HatRight(index, num);
		});

		return joyHat;
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

	int Joystick::Index()
	{
		return m_Index;
	}

	void Joystick::SetEventListener(const stJoystickEvent& listener)
	{
		Singleton<JoystickInternal>::GetInstance()->SetEventListener(m_Index, listener, this);
	}

	int Joystick::NumJoysticks()
	{
		return Singleton<JoystickInternal>::GetInstance()->NumJoysticks();
	}
}

