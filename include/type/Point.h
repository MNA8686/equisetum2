#if !defined(_EQPOINT_H_)
#define _EQPOINT_H_

#include <cstdint>
#include <cereal/cereal.hpp>

namespace Equisetum2
{
	template <typename T>
	struct Point_t
	{
		T x;
		T y;

		Point_t()
			: x(0), y(0)
		{
		}

		Point_t(T x_, T y_)
		{
			x = x_;
			y = y_;
		}

		Point_t(const Point_t<T>& src)
		{
			x = src.x;
			y = src.y;
		}

		Point_t<T>& operator += (const Point_t<T>& src)
		{
			x += src.x;
			y += src.y;
			return *this;
		}

		Point_t<T>& operator -= (const Point_t<T>& src)
		{
			x -= src.x;
			y -= src.y;
			return *this;
		}

		Point_t<T>& operator *= (const Point_t<T>& src)
		{
			x *= src.x;
			y *= src.y;
			return *this;
		}

		Point_t<T>& operator /= (const Point_t<T>& src)
		{
			x /= src.x;
			y /= src.y;
			return *this;
		}

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(x));
			archive(CEREAL_NVP(y));
		}
	};

	template <typename T>
	Point_t<T> operator += (const Point_t<T>& op1, const Point_t<T>& op2)
	{
		return{ op1.x + op2.x, op1.y + op2.y };
	}

	template <typename T>
	Point_t<T> operator -= (const Point_t<T>& op1, const Point_t<T>& op2)
	{
		return{ op1.x - op2.x, op1.y - op2.y };
	}

	template <typename T>
	Point_t<T> operator *= (const Point_t<T>& op1, const Point_t<T>& op2)
	{
		return{ op1.x * op2.x, op1.y * op2.y };
	}

	template <typename T>
	Point_t<T> operator /= (const Point_t<T>& op1, const Point_t<T>& op2)
	{
		return{ op1.x / op2.x, op1.y / op2.y };
	}

	template <typename T>
	bool operator == (const Point_t<T>& op1, const Point_t<T>& op2)
	{
		return op1.x == op2.x && op1.y == op2.y;
	}

	template <typename T>
	bool operator != (const Point_t<T>& op1, const Point_t<T>& op2)
	{
		return op1.x != op2.x || op1.y != op2.y;
	}

	using Point = Point_t<int32_t>;
	using PointF = Point_t<float>;
	using Size = Point_t<int32_t>;
	using SizeF = Point_t<float>;
}

#endif
