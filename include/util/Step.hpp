#if !defined(_EQSTEP_H_)
#define _EQSTEP_H_

#include "type/Point.h"
#include <cstdint>

namespace Equisetum2
{
	// 64ビットの数値に対応できるよう、template Cを設けている
	// T:ループごとにaddされていく数値 浮動小数点など、整数以外も可
	// C:ループカウンタ 整数以外は不可
	template<typename T = Point, typename C = decltype(Point::x),
		std::enable_if_t<std::is_integral<C>::value>* = nullptr>
//	template<typename T = Point, typename C = decltype(Point::x)>
	class Step_t
	{
	public:
		class iterator
		{
			T m_counter;	// 現在の座標
			T m_step;		// ステップ管理用
			decltype(T::x) m_startX = 0;	// xの初期値

			Point_t<C> m_stepCounter;				// ループ回数管理用
			decltype(Point_t<C>::x) m_countX = 0;	// xのループ回数初期値

		public:

			iterator() :
				m_stepCounter(0, 0)
			{
			}

			explicit iterator(Point_t<C> steps_count, T start, T step) :
				m_stepCounter(steps_count),
				m_countX(steps_count.x),
				m_counter(start),
				m_startX(start.x),
				m_step(step)
			{
			}

			iterator& operator++()
			{
				if (m_stepCounter.x == 1)
				{
					m_stepCounter.x = m_countX;
					m_stepCounter.y--;

					m_counter.x = m_startX;
					m_counter.y += m_step.y;
				}
				else
				{
					m_stepCounter.x--;

					m_counter.x += m_step.x;
				}

				return *this;
			}

			iterator operator++(int)
			{
				auto it = *this;

				++(*this);

				return it;
			}

			const T& operator*() const
			{
				return m_counter;
			}

			const T* operator->() const
			{
				return &m_counter;
			}

			bool operator== (const iterator& r) const
			{
				return m_stepCounter == r.m_stepCounter;
			}

			bool operator!= (const iterator& r) const
			{
				return !(m_stepCounter == r.m_stepCounter);
			}
		};

		explicit Step_t(T begin, Point_t<C> count, T step) :
			m_begin(count, begin, step),
			m_end({ count.x, 0 }, {}, {})
		{
		}

		explicit Step_t(T begin, Point_t<C> count) :
			Step_t(begin, count, T(1, 1))
		{
		}

		explicit Step_t(Point_t<C> count) :
			Step_t(T(0, 0), count, T(1, 1))
		{
		}

		explicit Step_t(decltype(Point_t<C>::x) count) :
			Step_t(T(0, 0), Point_t<C>(count, 1), T(1, 1))
		{
		}

		iterator begin() const
		{
			return m_begin;
		}

		iterator end() const
		{
			return m_end;
		}

	private:

		iterator m_begin;
		iterator m_end;
	};

	inline auto Step(Point begin, Point count, Point step)
	{
		return Step_t<>(begin, count, step);
	}

	inline auto Step(Point begin, Point count)
	{
		return Step_t<>(begin, count);
	}

	inline auto Step(Point count)
	{
		return Step_t<>(count);
	}

	inline auto Step(int32_t begin, int32_t count)
	{
		return Step_t<>({ begin, 0 },  { count, 1 });
	}

	inline auto Step(int32_t count)
	{
		return Step_t<>({ count, 1 });
	}

	inline auto Step(int64_t count)
	{
		return Step_t<Point_t<int64_t>, int64_t>({ count, 1 });
	}
}

#endif
