/**
* @file Regex.hpp
* @brief 正規表現を扱うクラス
* @author MNA
* @date 2017/04/12 新規作成
*/
#if !defined(_EQREGEX_H_)
#define _EQREGEX_H_

#include "type/String.h"
#include "type/Optional.h"
#include <vector>

namespace Equisetum2
{
	/**
	* 正規表現を扱うクラス
	*/
	class Regex
	{
	public:
		/**
		* @brief マッチした文字列を置換する(最初にマッチしたもののみ置換)
		* @param src 入力文字列
		* @param regex 検索する正規表現
		* @param replacement 置換後の正規表現
		* @return 置換後の文字列
		*/
		static String ReplaceFirst(const String& src, const String& regex, const String& replacement);

		/**
		* @brief マッチした文字列を置換する
		* @param src 入力文字列
		* @param regex 検索する正規表現
		* @param replacement 置換後の正規表現
		* @return 置換後の文字列
		*/
		static String ReplaceAll(const String& src, const String& regex, const String& replacement);

		/**
		* @brief 文字列を検索する
		* @param src 入力文字列
		* @param regex 検索する正規表現
		* @return 見つけた文字列
		* @detail 部分的に一致する文字列を探す際に使用する。
		*/
		static Optional<std::vector<String>> Search(const String& src, const String& regex);

		/**
		* @brief 文字列が一致するか判定する
		* @param src 入力文字列
		* @param regex 検索する正規表現
		* @return 一致した文字列
		* @detail 全体が一致するかどうかを判定する。
		*/
		static Optional<std::vector<String>> Match(const String& src, const String& regex);
	};
}

#endif
