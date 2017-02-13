#include "system/Logger.h"
#include "input/JoystickInternal.h"

#include "input/JoystickInternalImpl.inl"

namespace Equisetum2
{
	JoystickInternal::JoystickInternal()
	{
		m_Impl = std::make_shared<JoystickInternal::Impl>();
	}

	void JoystickInternal::Prepare()
	{
		m_Impl->Prepare();
	}

	void JoystickInternal::Update()
	{
		m_Impl->Update();
	}

	bool JoystickInternal::IsConnected(int joyIndex) const
	{
		return m_Impl->IsConnected(joyIndex);
	}

	int16_t JoystickInternal::Axis(int joyIndex, int num) const
	{
		return m_Impl->Axis(joyIndex, num);
	}

	HatState JoystickInternal::Hat(int joyIndex, int num) const
	{
		return m_Impl->Hat(joyIndex, num);
	}

	const stKeyState& JoystickInternal::Button(int joyIndex, int num) const
	{
		return m_Impl->Button(joyIndex, num);
	}

	int JoystickInternal::NumAxes(int joyIndex) const
	{
		return m_Impl->NumAxes(joyIndex);
	}

	int JoystickInternal::NumButtons(int joyIndex) const
	{
		return m_Impl->NumButtons(joyIndex);
	}

	int JoystickInternal::NumHats(int joyIndex) const
	{
		return m_Impl->NumHats(joyIndex);
	}
	
	const std::string JoystickInternal::Name(int joyIndex) const
	{
		return m_Impl->Name(joyIndex);
	}

	int JoystickInternal::NumJoysticks() const
	{
		return m_Impl->NumJoysticks();
	}
}

