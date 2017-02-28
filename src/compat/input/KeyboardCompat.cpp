#include "system/Logger.h"
#include "input/KeyboardCompat.h"

#include "input/KeyboardCompatImpl.inl"

namespace Equisetum2
{
	KeyboardCompat::KeyboardCompat()
	{
		m_Impl = std::make_shared<KeyboardCompat::Impl>();
	}

	void KeyboardCompat::Prepare()
	{
		m_Impl->Prepare();
	}

	void KeyboardCompat::Update()
	{
		m_Impl->Update();
	}

	const stKeyState& KeyboardCompat::GetKeyState(KeyCode keyCode) const
	{
		return m_Impl->GetKeyState(keyCode);
	}

	const String KeyboardCompat::GetNameFromKeyCode(KeyCode keyCode) const
	{
		return m_Impl->GetNameFromKeyCode(keyCode);
	}
}
