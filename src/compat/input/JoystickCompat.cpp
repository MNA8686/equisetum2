#include "system/Logger.h"
#include "input/JoystickCompat.h"

#include "input/JoystickCompatImpl.inl"

namespace Equisetum2
{
	JoystickCompat::JoystickCompat()
	{
		m_Impl = std::make_shared<JoystickCompat::Impl>();
	}

	void JoystickCompat::Prepare()
	{
		m_Impl->Prepare();
	}

	void JoystickCompat::Update()
	{
		m_Impl->Update();
	}

	bool JoystickCompat::IsConnected(int joyIndex) const
	{
		return m_Impl->IsConnected(joyIndex);
	}

	int16_t JoystickCompat::Axis(int joyIndex, int num) const
	{
		return m_Impl->Axis(joyIndex, num);
	}

	const stKeyState& JoystickCompat::HatUp(int joyIndex, int num) const
	{
		return m_Impl->HatUp(joyIndex, num);
	}

	const stKeyState& JoystickCompat::HatDown(int joyIndex, int num) const
	{
		return m_Impl->HatDown(joyIndex, num);
	}

	const stKeyState& JoystickCompat::HatLeft(int joyIndex, int num) const
	{
		return m_Impl->HatLeft(joyIndex, num);
	}

	const stKeyState& JoystickCompat::HatRight(int joyIndex, int num) const
	{
		return m_Impl->HatRight(joyIndex, num);
	}

	const stKeyState& JoystickCompat::Button(int joyIndex, int num) const
	{
		return m_Impl->Button(joyIndex, num);
	}

	int JoystickCompat::NumAxes(int joyIndex) const
	{
		return m_Impl->NumAxes(joyIndex);
	}

	int JoystickCompat::NumButtons(int joyIndex) const
	{
		return m_Impl->NumButtons(joyIndex);
	}

	int JoystickCompat::NumHats(int joyIndex) const
	{
		return m_Impl->NumHats(joyIndex);
	}
	
	const String JoystickCompat::Name(int joyIndex) const
	{
		return m_Impl->Name(joyIndex);
	}

	void JoystickCompat::SetEventListener(int joyIndex, const stJoystickEvent& listener, void* key)
	{
		return m_Impl->SetEventListener(joyIndex, listener, key);
	}

	int JoystickCompat::NumJoysticks() const
	{
		return m_Impl->NumJoysticks();
	}
}

