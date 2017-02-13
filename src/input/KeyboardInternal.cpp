#include "system/Logger.h"
#include "input/KeyboardInternal.h"

#include "input/KeyboardInternalImpl.inl"

namespace Equisetum2
{
	KeyboardInternal::KeyboardInternal()
	{
		m_Impl = std::make_shared<KeyboardInternal::Impl>();
	}

	void KeyboardInternal::Prepare()
	{
		m_Impl->Prepare();
	}

	void KeyboardInternal::Update()
	{
		m_Impl->Update();
	}

	const stKeyState& KeyboardInternal::GetKeyState(KeyCode keyCode) const
	{
		return m_Impl->GetKeyState(keyCode);
	}

	const std::string KeyboardInternal::GetNameFromKeyCode(KeyCode keyCode) const
	{
		return m_Impl->GetNameFromKeyCode(keyCode);
	}
}
