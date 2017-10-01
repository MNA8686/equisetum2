#if !defined(_EQPOINT_H_)
#define _EQPOINT_H_

namespace Equisetum2
{
	template <typename T>
	struct Point
	{
		T x;
		T y;

		Point()
			: x(0), y(0)
		{
		}

		Point(T x_, T y_)
		{
			x = x_;
			y = y_;
		}

		Point(const Point<T>& src)
		{
			x = src.x;
			y = src.y;
		}

		Point<T>& operator += (const Point<T>& src)
		{
			x += src.x;
			y += src.y;
			return *this;
		}

		Point<T>& operator -= (const Point<T>& src)
		{
			x -= src.x;
			y -= src.y;
			return *this;
		}

		Point<T>& operator *= (const Point<T>& src)
		{
			x *= src.x;
			y *= src.y;
			return *this;
		}

		Point<T>& operator /= (const Point<T>& src)
		{
			x /= src.x;
			y /= src.y;
			return *this;
		}
	};

	template <typename T>
	Point<T> operator += (const Point<T>& op1, const Point<T>& op2)
	{
		return{ op1.x + op2.x, op1.y + op2.y };
	}

	template <typename T>
	Point<T> operator -= (const Point<T>& op1, const Point<T>& op2)
	{
		return{ op1.x - op2.x, op1.y - op2.y };
	}

	template <typename T>
	Point<T> operator *= (const Point<T>& op1, const Point<T>& op2)
	{
		return{ op1.x * op2.x, op1.y * op2.y };
	}

	template <typename T>
	Point<T> operator /= (const Point<T>& op1, const Point<T>& op2)
	{
		return{ op1.x / op2.x, op1.y / op2.y };
	}

	template <typename T>
	Point<T> operator == (const Point<T>& op1, const Point<T>& op2)
	{
		return op1.x == op2.x && op1.y == op2.y;
	}

	template <typename T>
	Point<T> operator != (const Point<T>& op1, const Point<T>& op2)
	{
		return op1.x != op2.x || op1.y != op2.y;
	}

	using PointF = Point<float>;
	using Size = Point<int32_t>;
	using SizeF = Point<float>;
}

#endif
