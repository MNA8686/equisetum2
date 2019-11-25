/**
* @file Math.hpp
* @brief 数学関連寄せ集め
* @author MNA
* @date 2018/06/29 新規作成
*/
#if !defined(_EQMATH_H_)
#define _EQMATH_H_

#include <cstdint>
#include "type/FixedDec.hpp"
#include "stream/IStream.h"

namespace Equisetum2
{
	namespace FixedMath
	{
		// 固定小数点版三角関数型
		using FixedDegree = int32_t;
		const float PI = 3.14159265358979323846f;

		// 固定小数点版三角関数 小数部の桁数(標準は4096)
		const int32_t afterTheDecimalPoint = 12;
		// 角度の範囲: 0～4095
		const int32_t maxDegree = 1 << 12;
		// Sin,Cos,Tanなどの戻り値の範囲: -4096～4096
		const int32_t maxValue = 1 << 12;
		// Atanテーブルのサイズ(45度あたりの分解能) 比の範囲: 0～1024
		const int32_t atanTableSize = 1 << 10;

		void Init();
		int32_t Sin(FixedDegree degree);
		int32_t Cos(FixedDegree degree);
		FixedDegree Atan2(FixedDec y, FixedDec x);
		FixedDec Sqrt(FixedDec val);

		bool OutputSinTableToStream(std::shared_ptr<IStream> stream);
		bool OutputAtanTableToStream(std::shared_ptr<IStream> stream);

		FixedDec Sin(FixedDegree degree, const FixedDec& mul);
		FixedDec Cos(FixedDegree degree, const FixedDec& mul);
	}
}

#endif
