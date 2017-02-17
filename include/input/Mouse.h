#if !defined(_EQMOUSE_H_)
#define _EQMOUSE_H_

#include "input/IKeyState.h"
#include "input/Key.h"
#include "type/Point.h"
#include <memory>
#include <functional>

namespace Equisetum2
{
	enum class MouseButton : int
	{
		Left,
		Middle,
		Right,
		Max
	};

	class Mouse
	{
	public:
		static Point<int32_t> Pos();
		static Point<int32_t> Delta();
		static int32_t Wheel();
		static int32_t WheelH();
		static const Key Button(MouseButton button = MouseButton::Left);
		static bool ShowCursor(bool toggle);
	};
}

#endif
