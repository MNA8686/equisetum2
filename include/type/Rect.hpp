#if !defined(_EQRECT_H_)
#define _EQRECT_H_

#include <cstdint>

namespace Equisetum2
{
	template <typename T>
	struct Rect_t
	{
		T x;
		T y;
		T width;
		T height;

		Rect_t()
			: x(0), y(0), width(0), height(0)
		{
		}

		Rect_t(T x_, T y_, T width_, T height_)
		{
			x = x_;
			y = y_;
			width = width_;
			height = height_;
		}

		Rect_t(const Rect_t<T>& src)
		{
			x = src.x;
			y = src.y;
			width = src.width;
			height = src.height;
		}
	};

	template <typename T>
	Rect_t<T> operator == (const Rect_t<T>& op1, const Rect_t<T>& op2)
	{
		return op1.x == op2.x &&
			op1.y == op2.y &&
			op1.width == op2.width &&
			op1.height == op2.height;
	}

	template <typename T>
	Rect_t<T> operator != (const Rect_t<T>& op1, const Rect_t<T>& op2)
	{
		return !(op1 == op2);
	}

	using Rect = Rect_t<int32_t>;
	using RectF = Rect_t<float>;
}

#endif
