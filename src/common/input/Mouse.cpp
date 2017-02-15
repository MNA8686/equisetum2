#include "input/Mouse.h"
#include "system/Logger.h"
#include "input/MouseCompat.h"

namespace Equisetum2
{
	std::shared_ptr<Mouse> Mouse::Create()
	{
		class MouseDummy : public Mouse
		{
		};
		return std::make_shared<MouseDummy>();
	}

	Mouse::Mouse()
	{
	}

	Mouse::~Mouse()
	{
	}

	Point Mouse::Pos() const
	{
		return Singleton<MouseCompat>::GetInstance()->Pos();
	}

	Point Mouse::Delta() const
	{
		return Singleton<MouseCompat>::GetInstance()->Delta();
	}

	int32_t Mouse::Wheel() const
	{
		return Singleton<MouseCompat>::GetInstance()->Wheel();
	}

	int32_t Mouse::WheelH() const
	{
		return Singleton<MouseCompat>::GetInstance()->WheelH();
	}

	const Key Mouse::Button(MouseButton button) const
	{
		const Key mouseButton([button]()->stKeyState
		{
			return Singleton<MouseCompat>::GetInstance()->Button(button);
		});

		return mouseButton;
	}

	bool Mouse::ShowCursor(bool toggle)
	{
		return Singleton<MouseCompat>::GetInstance()->ShowCursor(toggle);
	}
}

