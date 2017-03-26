#include "graphic/Window.h"
#include "system/Logger.h"
#include "graphic/WindowCompat.h"

namespace Equisetum2
{
	void Window::SetTitle(const String& title)
	{
		Singleton<WindowCompat>::GetInstance()->SetTitle(title);
	}

	void Window::SetSize(int width, int height)
	{
		Singleton<WindowCompat>::GetInstance()->SetSize(width, height);
	}

	void Window::SetStyle(WindowStyle style)
	{
		Singleton<WindowCompat>::GetInstance()->SetStyle(style);
	}

	void Window::SetPos(int x, int y)
	{
		Singleton<WindowCompat>::GetInstance()->SetPos(x, y);
	}

	void Window::Centering()
	{
		Singleton<WindowCompat>::GetInstance()->Centering();
	}

	void Window::ShowAlert(const String& msg)
	{
		Singleton<WindowCompat>::GetInstance()->ShowAlert(msg);
	}

	const String Window::Title()
	{
		return Singleton<WindowCompat>::GetInstance()->Title();
	}

	Size Window::Size()
	{
		return Singleton<WindowCompat>::GetInstance()->Size();
	}

	Point<int> Window::Pos()
	{
		return Singleton<WindowCompat>::GetInstance()->Pos();
	}

	WindowStyle Window::Style()
	{
		return Singleton<WindowCompat>::GetInstance()->Style();
	}
}

