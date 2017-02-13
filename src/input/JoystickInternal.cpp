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

	const stKeyState& JoystickInternal::HatUp(int joyIndex, int num) const
	{
		return m_Impl->HatUp(joyIndex, num);
	}

	const stKeyState& JoystickInternal::HatDown(int joyIndex, int num) const
	{
		return m_Impl->HatDown(joyIndex, num);
	}

	const stKeyState& JoystickInternal::HatLeft(int joyIndex, int num) const
	{
		return m_Impl->HatLeft(joyIndex, num);
	}

	const stKeyState& JoystickInternal::HatRight(int joyIndex, int num) const
	{
		return m_Impl->HatRight(joyIndex, num);
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

	void JoystickInternal::SetEventListener(int joyIndex, const stJoystickEvent& listener, void* key)
	{
		return m_Impl->SetEventListener(joyIndex, listener, key);
	}

	int JoystickInternal::NumJoysticks() const
	{
		return m_Impl->NumJoysticks();
	}
}

