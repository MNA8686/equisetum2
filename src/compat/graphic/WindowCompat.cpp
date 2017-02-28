#include "graphic/Window.h"
#include "system/Logger.h"
#include "graphic/WindowCompat.h"

#include "graphic/WindowCompatImpl.inl"

namespace Equisetum2
{
	WindowCompat::WindowCompat()
	{
		m_Impl = std::make_shared<WindowCompat::Impl>();
	}

	void WindowCompat::Prepare()
	{
		m_Impl->Prepare();
	}

	void WindowCompat::Update()
	{
		m_Impl->Update();
	}


	void WindowCompat::SetTitle(const std::string& title)
	{
		m_Impl->SetTitle(title);
	}

	void WindowCompat::SetSize(int width, int height)
	{
		m_Impl->SetSize(width, height);
	}

	void WindowCompat::SetStyle(WindowStyle style)
	{
		m_Impl->SetStyle(style);
	}

	void WindowCompat::SetPos(int x, int y)
	{
		m_Impl->SetPos(x, y);
	}

	void WindowCompat::Centering()
	{
		m_Impl->Centering();
	}

	const std::string WindowCompat::Title()
	{
		return m_Impl->Title();
	}

	Equisetum2::Size WindowCompat::Size()
	{
		return m_Impl->Size();
	}

	Point<int> WindowCompat::Pos()
	{
		return m_Impl->Pos();
	}

	WindowStyle WindowCompat::Style()
	{
		return m_Impl->Style();
	}
}

