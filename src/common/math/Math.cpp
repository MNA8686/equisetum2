/**
* @file Math.cpp
* @brief 数学関連寄せ集め
* @author MNA
* @date 2018/06/29 新規作成
*/

#include "system/Exception.hpp"
#include "system/Logger.h"
#include "stream/TextWriter.h"
#include "math/Math.hpp"

namespace Equisetum2
{
	namespace FixedMath
	{
#include "SinTable.inl"
#include "AtanTable.inl"

		bool OutputSinTableToStream(std::shared_ptr<IStream> stream)
		{
			bool ret = false;

			if (stream)
			{
				if (auto writer = TextWriter::CreateFromStream(stream))
				{
					writer->WriteLine("const int32_t g_sinTable[] =");
					writer->WriteLine("{");

					String out;
					int index = 0;

					// 90度分のsinテーブルを出力する
					for (int i = 0; i < maxDegree / 4; i++)
					{
						const auto val = static_cast<int32_t>(sin(PI * i / (maxDegree / 2)) * maxValue);
						out += String::Sprintf("%s%4d,", index == 0 ? "\t" : " ", val);

						index++;

						if (index == 16)
						{
							writer->WriteLine(out);
							out.clear();
							index = 0;
						}
					}

					if (!out.empty())
					{
						writer->WriteLine(out);
					}

					writer->WriteLine("};");

					ret = true;
				}
			}

			return ret;
		}

		bool OutputAtanTableToStream(std::shared_ptr<IStream> stream)
		{
			bool ret = false;

			if (stream)
			{
				if (auto writer = TextWriter::CreateFromStream(stream))
				{
					writer->WriteLine("const FixedDegree g_atanTable[] =");
					writer->WriteLine("{");

					String out;
					int index = 0;

					// 45度分のテーブルを出力する
					for (int i = 0; i <= atanTableSize; i++)
					{
						const auto val = static_cast<int32_t>(atan(i / static_cast<float>(atanTableSize)) * (maxDegree / 2) / PI);
						out += String::Sprintf("%s/* %4d */%4d,", index == 0 ? "\t" : " ", i, val);

						index++;

						if (index == 16)
						{
							writer->WriteLine(out);
							out.clear();
							index = 0;
						}
					}

					if (!out.empty())
					{
						writer->WriteLine(out);
					}

					writer->WriteLine("};");

					ret = true;
				}
			}

			return ret;
		}

		int32_t Sin(FixedDegree degree)
		{
			const FixedDegree quoit = maxDegree / 4;
			int32_t retVal = 0;

			degree &= (maxDegree - 1);

			if (degree < quoit)
			{
				retVal = g_sinTable[degree];
			}
			else if (degree < quoit * 2)
			{
				auto d = degree - quoit;
				retVal = g_sinTable[quoit - 1 - d];
			}
			else if (degree < quoit * 3)
			{
				auto d = degree - quoit * 2;
				retVal = -g_sinTable[d];
			}
			else
			{
				auto d = degree - quoit * 3;
				retVal = -g_sinTable[quoit - 1 - d];
			}

			return retVal;
		}

		int32_t Cos(FixedDegree degree)
		{
			const FixedDegree quoit = maxDegree / 4;
			return Sin(degree + quoit);
		}

		FixedDegree Atan2(FixedDec y, FixedDec x)
		{
			const FixedDegree quoit = maxDegree / 4;
			FixedDegree retVal = 0;

			const int32_t x_ = x.GetRaw();
			const int32_t y_ = y.GetRaw();

			// IEC 60559準拠のようなもの
			if (y_ == 0)
			{
				if (x_ >= 0)
				{
					// xとyが両方0の場合もここに来る
					retVal = 0;
				}
				else
				{
					retVal = quoit * 2;
				}

				return retVal;
			}
			if (x_ == 0)
			{
				if (y_ > 0)
				{
					retVal = quoit;
				}
				else
				{
					retVal = quoit * 3;
				}

				return retVal;
			}

			if (x_ > 0)
			{
				// 第1象限
				if (y_ > 0)
				{
					if (y_ < x_)
					{
						const int32_t ratio = y_ * atanTableSize / x_;
						retVal = g_atanTable[ratio];
					}
					else
					{
						const int32_t ratio = x_ * atanTableSize / y_;
						retVal = quoit - g_atanTable[ratio];
					}
				}
				// 第4象限
				else if (y_ < 0)
				{
					if (-y_ < x_)
					{
						const int32_t ratio = -y_ * atanTableSize / x_;
						retVal = quoit * 4 - g_atanTable[ratio];
					}
					else
					{
						const int32_t ratio = x_ * atanTableSize / -y_;
						retVal = quoit * 3 + g_atanTable[ratio];
					}
				}
			}
			else
			{
				// 第2象限
				if (y_ > 0)
				{
					if (y_ < -x_)
					{
						const int32_t ratio = y_ * atanTableSize / -x_;
						retVal = quoit * 2 - g_atanTable[ratio];
					}
					else
					{
						const int32_t ratio = -x_ * atanTableSize / y_;
						retVal = quoit + g_atanTable[ratio];
					}
				}
				// 第3象限
				else if (y_ < 0)
				{
					if (-y_ < -x_)
					{
						const int32_t ratio = -y_ * atanTableSize / -x_;
						retVal = quoit * 2 + g_atanTable[ratio];
					}
					else
					{
						const int32_t ratio = -x_ * atanTableSize / -y_;
						retVal = quoit * 3 - g_atanTable[ratio];
					}
				}
			}

			return retVal;
		}

		FixedDegree NormalizeDegree(FixedDegree degree)
		{
			const int32_t mask = (maxDegree - 1);

			const FixedDegree masked = degree & mask;

			return masked;
		}

		int32_t GetNearDirection(FixedDegree srcDegree, FixedDegree dstDegree)
		{
			int32_t direction = 0;
			const FixedDegree srcDegree_ = NormalizeDegree(srcDegree);
			const FixedDegree dstDegree_ = NormalizeDegree(dstDegree);

			if (srcDegree_ != dstDegree_)
			{
				const int32_t right = (maxDegree + srcDegree_ - dstDegree_) % maxDegree;
				const int32_t left = (maxDegree + dstDegree_ - srcDegree_) % maxDegree;

				if (left <= right)
				{
					direction = 1;
				}
				else if (left > right)
				{
					direction = -1;
				}
			}

			return direction;
		}

		FixedDec Sqrt(FixedDec val)
		{
			const FixedDec div = 2;
			FixedDec x = val / div;
			FixedDec lastX = 0;

			while (x != lastX)
			{
				lastX = x;
				x = (x + val / x) / div;
			}

			return x;
		}

		FixedDec Sin(FixedDegree degree, const FixedDec& mul)
		{
			FixedDec ret;
			ret.SetRaw((FixedMath::Sin(degree) * mul.GetRaw()) >> FixedMath::afterTheDecimalPoint);
			return ret;
		}

		FixedDec Cos(FixedDegree degree, const FixedDec& mul)
		{
			FixedDec ret;
			ret.SetRaw((FixedMath::Cos(degree) * mul.GetRaw()) >> FixedMath::afterTheDecimalPoint);
			return ret;
		}
	}
}

