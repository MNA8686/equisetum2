/**
* @file Directory.hpp
* @brief ディレクトリ操作に関するクラス
* @author MNA
* @date 2017/04/12 新規作成
*/
#if !defined(_EQDIRECTORY_H_)
#define _EQDIRECTORY_H_

#include "type/String.h"
#include "type/Optional.h"

namespace Equisetum2
{
	/**
	* ディレクトリ操作に関するクラス
	*/
	class Directory
	{
	public:

		/**
		* @brief ディレクトリを作成する
		* @param path ディレクトリ名
		* @return 成否
		*/
		static bool Create(const String& path);

		/**
		* @brief ディレクトリを削除する
		* @param path ディレクトリ名
		* @return 成否
		*
		* @detail 削除するディレクトリは空で無ければならない
		*/
		static bool Delete(const String& path, bool recursion=false);

		/**
		* @brief ディレクトリが存在するか確認する
		* @param path ディレクトリ名
		* @return 存在有無
		*/
		static bool Exists(const String& path);

		/**
		* @brief ディレクトリを移動する
		* @param src 移動元ディレクトリ名
		* @param dst 移動先ディレクトリ名
		* @return 成否
		*/
		static bool Move(const String& src, const String& dst);

		/**
		* @brief ディレクトリ内のファイルを列挙する 1件列挙するごとにコールバックが呼び出される
		* @param path 対象ディレクトリ名
		* @param cb コールバック 引数に列挙したファイル名が入る。 trueを返すと列挙はそこで終了する。
		* @return 成否
		*/
		static bool EnumerateFiles(const String& path, const std::function<bool(const String&)> cb);

		/**
		* @brief ディレクトリ内の全てのファイルを列挙する
		* @param path 対象ディレクトリ名
		* @return ファイル名の配列
		*/
		static Optional<std::vector<String>> GetFiles(const String& path, bool recursion=false);

		/**
		* @brief ディレクトリ内のディレクトリを列挙する 1件列挙するごとにコールバックが呼び出される
		* @param path 対象ディレクトリ名
		* @param cb コールバック 引数に列挙したディレクトリ名が入る。 trueを返すと列挙はそこで終了する。
		* @return 成否
		*
		* @detail 取得したパスの終端はセパレータであることが保証されている
		*/
		static bool EnumerateDirectories(const String& path, const std::function<bool(const String&)> cb);

		/**
		* @brief ディレクトリ内の全てのディレクトリを列挙する
		* @param path 対象ディレクトリ名
		* @return ディレクトリ名の配列
		*
		* @detail 取得したパスの終端はセパレータであることが保証されている
		*/
		static Optional<std::vector<String>> GetDirectories(const String& path, bool recursion=false);
	};
}

#endif
