#if !defined(_EQMOUSE_H_)
#define _EQMOUSE_H_

#include "input/IKeyState.h"
#include "input/Key.h"
#include <memory>
#include <functional>

namespace Equisetum2
{
	typedef struct
	{
		int32_t x;
		int32_t y;
	}Point;

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

		static std::shared_ptr<Mouse> Create();

		Point Pos() const;
		Point Delta() const;
		int32_t Wheel() const;
		int32_t WheelH() const;
		const Key Button(MouseButton button) const;

		bool ShowCursor(bool toggle);

		~Mouse();

	protected:

		Mouse();	// インスタンス作成封じ
	};
}

#endif
