/**
* @file String.h
* @brief UTF-8を使うのに特化したstd::string
* @author MNA
* @date 2017/02/28 新規作成
*/
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
	/**
	* UTF-8を使うのに特化したstd::string
	*/
	class String : public std::string
	{
	public:

		/**
		* @brief 空の文字列を作成
		*/
		String() {}

		/**
		* @brief C文字列を元にUTF-8文字列を作成
		* @param str C文字列
		*/
		String(const char* str) : std::string(str) {}

		/**
		* @brief std::stringを元にUTF-8文字列を作成
		* @param str std::string
		*/
		String(const std::string& str) : std::string(str) {}

		/**
		* @brief std::u16stringを元にUTF-8文字列を作成
		* @param str std::u16string
		*/
		String(const std::u16string& str) { from_u16(str); }

		/**
		* @brief std::u32stringを元にUTF-8文字列を作成
		* @param str std::u32string
		*/
		String(const std::u32string& str) { from_u32(str); }

		/**
		* @brief std::Stringを元にUTF-8文字列を作成
		* @param str std::String
		*/
		String(const String& str) : std::string(str) {}

		/**
		* @brief コードポイント単位のサイズを返す
		* @return コードポイント数
		*
		* @detail 例: u8"ハム太郎" size() => 12, size_by_codepoints() => 4
		*/
		size_type size_by_codepoints() const noexcept
		{
			return to_u32().size();
		}

		/**
		* @brief コードポイント単位でsubstrを行う
		* @param pos 取り出し開始位置
		* @param len 取り出しコードポイント数
		* @return 取り出したUTF-8文字列
		*/
		String substr_by_codepoints(size_t pos = 0, size_t len = std::string::npos) const
		{
			return{ to_u32().substr(pos, len) };
		}

		/**
		* @brief コードポイント単位でatを行う
		* @param offset 取得位置
		* @return 取り出したUTF-8文字(1コードポイント)
		*/
		String at_by_codepoints(size_t offset) const
		{
			return std::u32string{ to_u32().at(offset) };
		}

		/**
		* @brief UTF-32からUTF-8文字列を作成する
		* @param src UTF-32文字列
		* @return 自身への参照
		*/
		String& from_u32(const std::u32string& src)
		{
			// VC++のバグ対策でいろいろキャストしてる
			// 本来は int32_t -> char32_t
#ifdef _MSC_VER
            std::wstring_convert<std::codecvt_utf8<int32_t>, int32_t> conv;
			auto u8str = conv.to_bytes((const int32_t*)src.c_str());
#else
            std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
            auto u8str = conv.to_bytes((const char32_t*)src.c_str());
#endif
            assign(u8str);

			return *this;
		}

		/**
		* @brief UTF-8からUTF-32文字列を作成する
		* @return UTF-32文字列
		*/
		std::u32string to_u32() const
		{
			// VC++のバグ対策でいろいろキャストしてる
			// 本来は int32_t -> char32_t
#ifdef _MSC_VER
			std::wstring_convert<std::codecvt_utf8<int32_t>, int32_t> conv;
#else
            std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
#endif
            auto u32str = conv.from_bytes(*this);
            return{ (const char32_t*)u32str.c_str() };
        }

		/**
		* @brief UTF-16からUTF-8文字列を作成する
		* @param src UTF-16文字列
		* @return 自身への参照
		*/
		String& from_u16(const std::u16string& src)
		{
			// VC++のバグ対策でいろいろキャストしてる
			// 本来は wchar_t -> char16_t (UTF8への変換時はwchar_tにしないと何故かクラッシュする)
			// Windowsではwchar_tは16ビットだが、他の環境では32ビットだったりするので注意
			std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> conv;
			auto u8str = conv.to_bytes((const wchar_t*)src.c_str());
			assign(u8str);

			return *this;
		}

		/**
		* @brief UTF-8からUTF-16文字列を作成する
		* @return UTF-16文字列
		*/
		std::u16string to_u16() const
		{
			// VC++のバグ対策でいろいろキャストしてる
			// 本来は int16_t -> char16_t
#ifdef _MSC_VER
			std::wstring_convert<std::codecvt_utf8<int16_t>, int16_t> conv;
#else
            std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> conv;
#endif
            auto u16str = conv.from_bytes(*this);
			return{ (const char16_t*)u16str.c_str() };
		}

		/**
		* @brief UTF-8をformatする
		* @param fmt UTF8文字列
		* @return 自身への参照
		*/
		String& format(const char* fmt, ...)
		{
			va_list argList;
			va_start(argList, fmt);
			formatV(fmt, argList);
			va_end(argList);

			return *this;
		}

		/**
		* @brief UTF-8をformatする
		* @param fmt UTF8文字列
		* @return 自身への参照
		*/
		String& formatV(const char* fmt, va_list argList)
		{
			va_list copy;
			va_copy(copy, argList);

			auto return_value = vsnprintf(nullptr, 0, fmt, argList);
			if (return_value >= 0)
			{
				std::vector<char> buf(return_value + 1);

				vsnprintf(&buf[0], return_value + 1, fmt, copy);
				this->assign(&buf[0]);
			}

			return *this;
		}

		/**
		* @brief 前後のスペースを取り除く
		* @param separator スペースとして認識するUTF8文字列
		* @return トリムされた文字列
		*/
		String trim(const String& separator = " \t")
		{
			auto str32 = to_u32();
			auto isSpace32 = separator.to_u32();
			String trimed;

			// 左側からトリムする文字以外が見つかる位置を検索
			auto left = str32.find_first_not_of(isSpace32);
			if (left != std::u32string::npos)
			{
				// 右からトリムする文字以外が見つかる位置を検索
				auto right = str32.find_last_not_of(isSpace32);
				if (right != std::u32string::npos)
				{
					auto result = str32.substr(left, right - left + 1);
					trimed.from_u32(result);
				}
			}

			return trimed;
		}

		/**
		* @brief 与えられたセパレータを元に文字列を分割する
		* @param separator デリミタとして認識するUTF8文字列
		* @param compressBlank 真の場合、空白を無視する
		* @return 分割された文字列の配列
		*/
		std::vector<String> split(const String& separator = " \t", bool compressBlank = true)
		{
			auto str32 = to_u32();
			auto isSpace32 = separator.to_u32();
			std::u32string::size_type pos = 0;

			std::vector<String> v;

			while (true)
			{
				auto delimPos = str32.find_first_of(isSpace32, pos);
				if (delimPos == std::u32string::npos)
				{
					auto sub = str32.substr(pos, std::u32string::npos);
					if (!compressBlank || sub.size() > 0)
					{
						v.push_back(String(sub));
					}
					break;
				}
				else
				{
					if (!compressBlank || delimPos - pos > 0)
					{
						auto sub = str32.substr(pos, delimPos - pos);
						v.push_back(String(sub));
					}

					pos = delimPos + 1;
				}
			}

			return v;
		}

		/**
		* @brief UTF-8をformatし、新たなStringとして返す
		* @param fmt UTF8文字列
		* @return 作成したString
		*/
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
