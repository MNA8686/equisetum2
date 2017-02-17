#include "input/Mouse.h"
#include "system/Logger.h"
#include "input/MouseCompat.h"

namespace Equisetum2
{
	Point<int32_t> Mouse::Pos()
	{
		return Singleton<MouseCompat>::GetInstance()->Pos();
	}

	Point<int32_t> Mouse::Delta()
	{
		return Singleton<MouseCompat>::GetInstance()->Delta();
	}

	int32_t Mouse::Wheel()
	{
		return Singleton<MouseCompat>::GetInstance()->Wheel();
	}

	int32_t Mouse::WheelH()
	{
		return Singleton<MouseCompat>::GetInstance()->WheelH();
	}

	const Key Mouse::Button(MouseButton button)
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

