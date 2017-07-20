#include "system/Logger.h"
#include "input/MouseCompat.h"

#include "input/MouseCompatImpl.inl"

namespace Equisetum2
{
	MouseCompat::MouseCompat()
	{
		m_Impl = std::make_shared<MouseCompat::Impl>();
	}

	void MouseCompat::Prepare()
	{
		m_Impl->Prepare();
	}

	void MouseCompat::Update()
	{
		m_Impl->Update();
	}

	const Point<int32_t>& MouseCompat::Pos() const
	{
		return m_Impl->Pos();
	}

	const Point<int32_t>& MouseCompat::Delta() const
	{
		return m_Impl->Delta();
	}

	int32_t MouseCompat::Wheel() const
	{
		return m_Impl->Wheel();
	}

	int32_t MouseCompat::WheelH() const
	{
		return m_Impl->WheelH();
	}

	const stKeyState& MouseCompat::Button(MouseButton button) const
	{
		return m_Impl->Button(button);
	}

	bool MouseCompat::ShowCursor(bool toggle)
	{
		return m_Impl->ShowCursor(toggle);
	}
}
