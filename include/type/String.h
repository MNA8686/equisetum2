#if !defined(_EQSTRING_H_)
#define _EQSTRING_H_

#include <stdarg.h> 
#include <cstdint>
#include <string>
#include <locale>
#include <codecvt>
#include <vector>

namespace Equisetum2
{
	// UTF8を扱うための文字列クラス

	class String : public std::string
	{
	public:

		String() {}
		String(const char* str) : std::string(str) {}
		String(const std::string& str) : std::string(str) {}
		String(const std::u16string& str) { from_u16(str); }
		String(const std::u32string& str) { from_u32(str); }
		String(const String& str) : std::string(str) {}

		// コードポイント単位のサイズを返す
		// u8"ハム太郎" size() => 12, size_by_codepoints() => 4
		size_type size_by_codepoints() const noexcept
		{
			return to_u32().size();
		}

		String substr_by_codepoints(size_t pos = 0, size_t len = std::string::npos) const
		{
			return{ to_u32().substr(pos, len) };
		}

		void from_u32(const std::u32string& src)
		{
			// VC++のバグ対策でいろいろキャストしてる
			// 本来は int32_t -> char32_t
			std::wstring_convert<std::codecvt_utf8<int32_t>, int32_t> conv;
			auto u8str = conv.to_bytes((const int32_t*)src.c_str());
			assign(u8str);
		}

		std::u32string to_u32() const
		{
			// VC++のバグ対策でいろいろキャストしてる
			// 本来は int32_t -> char32_t
			std::wstring_convert<std::codecvt_utf8<int32_t>, int32_t> conv;
			auto u32str = conv.from_bytes(*this);
			return{ (const char32_t*)u32str.c_str() };
		}

		void from_u16(const std::u16string& src)
		{
			// VC++のバグ対策でいろいろキャストしてる
			// 本来は wchar_t -> char16_t (UTF8への変換時はwchar_tにしないと何故かクラッシュする)
			// Windowsではwchar_tは16ビットだが、他の環境では32ビットだったりするので注意
			std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> conv;
			auto u8str = conv.to_bytes((const wchar_t*)src.c_str());
			assign(u8str);
		}

		std::u16string to_u16() const
		{
			// VC++のバグ対策でいろいろキャストしてる
			// 本来は int16_t -> char16_t
			std::wstring_convert<std::codecvt_utf8<int16_t>, int16_t> conv;
			auto u16str = conv.from_bytes(*this);
			return{ (const char16_t*)u16str.c_str() };
		}

		void format(const char* fmt, ...)
		{
			va_list argList;
			va_start(argList, fmt);
			formatV(fmt, argList);
			va_end(argList);
		}

		void formatV(const char* fmt, va_list argList)
		{
			va_list copy;
			va_copy(copy, argList);

			auto return_value = vsnprintf(nullptr, 0, fmt, argList);
			if (return_value > 0)
			{
				std::vector<char> buf(return_value + 1);

				vsnprintf(&buf[0], return_value + 1, fmt, copy);
				this->assign(&buf[0]);
			}
		}

		static String Sprintf(const char* fmt, ...)
		{
			String str;

			va_list argList;
			va_start(argList, fmt);
			str.formatV(fmt, argList);
			va_end(argList);

			return str;
		}
	};

	inline String operator+(const String& str1, const String& str2)
	{
		String strRet(str1);
		strRet.append(str2);
		return strRet;
	}

	inline String operator+(const String& str1, const char* str2)
	{
		String strRet(str1);
		strRet.append(str2);
		return strRet;
	}

	inline String operator+(const char* str1, const String& str2)
	{
		String strRet(str1);
		strRet.append(str2);
		return strRet;
	}
}

#endif
