#if !defined(_EQFIXEDDEC_H_)
#define _EQFIXEDDEC_H_

#include <cstdint>
#include <cereal/cereal.hpp>

namespace Equisetum2
{
	class FixedDec final
	{
	public:
		// 小数部の桁数
		static const int afterTheDecimalPoint = 8;

		// RAWデータからFixedDecを生成する。
		// もし将来小数部の桁数が変わってもFixedDec::Rawしている箇所を探せば修正可能となる。
		inline static FixedDec Raw(int32_t rawVal)
		{
			FixedDec val;
			val.SetRaw(rawVal);
			return val;
		}

		inline FixedDec() = default;

		inline FixedDec(int32_t val)
		{
			m_val = val << afterTheDecimalPoint;
		}

		inline FixedDec(const FixedDec& src)
		{
			m_val = src.m_val;
		}

		inline FixedDec(float val) : FixedDec(static_cast<double>(val))
		{
		}

		inline FixedDec(double val)
		{
			const int32_t mul = 1 << afterTheDecimalPoint;
			m_val = static_cast<int32_t>(val * static_cast<double>(mul));
		}

		inline FixedDec& operator = (const FixedDec& op1)
		{
			m_val = op1.m_val;
			return *this;
		}

		inline FixedDec& operator += (const FixedDec& op1)
		{
			m_val += op1.m_val;
			return *this;
		}

		inline 	FixedDec& operator -= (const FixedDec& op1)
		{
			m_val -= op1.m_val;
			return *this;
		}

		inline FixedDec& operator *= (const FixedDec& op1)
		{
			int64_t val64 = static_cast<int64_t>(m_val) * static_cast<int64_t>(op1.m_val);
			m_val = static_cast<int32_t>(val64 >> afterTheDecimalPoint);
			return *this;
		}

		inline FixedDec& operator /= (const FixedDec& op1)
		{
			const int32_t mul = 1 << afterTheDecimalPoint;
			int64_t val64 = static_cast<int64_t>(m_val) * static_cast<int64_t>(mul);

			m_val = static_cast<int32_t>(val64 / static_cast<int64_t>(op1.m_val));
			return *this;
		}

		inline FixedDec& operator /= (const int32_t& op1)
		{
			m_val /= op1;
			return *this;
		}

// 一応小数なのでインクリメントとデクリメントは不要では？
#if 0
		inline FixedDec operator ++()
		{
			*this += FixedDec(1);
			return *this;
		}

		inline FixedDec operator --()
		{
			*this -= FixedDec(1);
			return *this;
		}

		inline FixedDec operator ++(int)
		{
			*this += FixedDec(1);
			return *this;
		}

		inline FixedDec operator --(int)
		{
			*this -= FixedDec(1);
			return *this;
		}
#endif

		inline bool operator > (const FixedDec& op1) const
		{
			return m_val > op1.m_val;
		}
		inline bool operator >= (const FixedDec& op1) const
		{
			return m_val >= op1.m_val;
		}
		inline bool operator < (const FixedDec& op1) const
		{
			return m_val < op1.m_val;
		}
		inline bool operator <= (const FixedDec& op1) const
		{
			return m_val <= op1.m_val;
		}

		inline bool operator == (const FixedDec& op1) const
		{
			return m_val == op1.m_val;
		}

		inline bool operator != (const FixedDec& op1) const
		{
			return !(*this == op1);
		}

		inline operator bool() const
		{
			return m_val != 0;
		}

		inline int32_t GetInt() const
		{
			return m_val >> afterTheDecimalPoint;
		}

		inline void SetRaw(int32_t val)
		{
			m_val = val;
		}

		inline int32_t GetRaw() const
		{
			return m_val;
		}

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(m_val));
		}

	private:
		int32_t m_val = 0;
	};

	inline FixedDec operator+(const FixedDec& op1, const FixedDec& op2)
	{
		auto fixedVal = op1;
		fixedVal += op2;
		return fixedVal;
	}

	inline FixedDec operator-(const FixedDec& op1, const FixedDec& op2)
	{
		auto fixedVal = op1;
		fixedVal -= op2;
		return fixedVal;
	}

	inline FixedDec operator*(const FixedDec& op1, const FixedDec& op2)
	{
		auto fixedVal = op1;
		fixedVal *= op2;
		return fixedVal;
	}

	inline FixedDec operator/(const FixedDec& op1, const FixedDec& op2)
	{
		auto fixedVal = op1;
		fixedVal /= op2;
		return fixedVal;
	}

	inline FixedDec operator/(const FixedDec& op1, const int32_t& op2)
	{
		auto fixedVal = op1;
		fixedVal /= op2;
		return fixedVal;
	}
}

#endif
