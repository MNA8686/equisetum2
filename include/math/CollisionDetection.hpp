/**
* @file CollisionDetection.hpp
* @brief 当たり判定関連寄せ集め
* @author MNA
* @date 2019/12/03 新規作成
*/
#if !defined(_EQCOLLISIONDETECTION_H_)
#define _EQCOLLISIONDETECTION_H_

#include <cstdint>
#include "type/FixedDec.hpp"
#include "type/Rect.hpp"

namespace Equisetum2
{
	namespace CollisionDetection
	{
		template <typename T>
		bool HitTest(const Rect_t<T>& op1, const Rect_t<T>& op2)
		{
			if (op1.x < op2.x + op2.width &&
				op1.x + op1.width > op2.x &&
				op1.y < op2.y + op2.height &&
				op1.y + op1.height > op2.y)
			{
				return true;
			}

			return false;
		}
	}
}

#endif
