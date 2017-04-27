/**
* @file File.hpp
* @brief ファイル操作に関するクラス
* @author MNA
* @date 2017/04/12 新規作成
*/
#if !defined(_EQFILE_H_)
#define _EQFILE_H_

#include "type/String.h"

namespace Equisetum2
{
	/**
	* ファイル操作に関するクラス
	*/
	class File
	{
	public:

		/**
		* @brief ファイルを作成する
		* @param path ファイル名
		* @return 成否
		*/
		static bool Create(const String& path);

		/**
		* @brief ファイルを削除する
		* @param path ファイル名
		* @return 成否
		*/
		static bool Delete(const String& path);

		/**
		* @brief ファイルが存在するか確認する
		* @param path ファイル名
		* @return 存在有無
		*/
		static bool Exists(const String& path);

		/**
		* @brief ファイルを移動する
		* @param src 移動元ファイル名
		* @param dst 移動先ファイル名
		* @return 成否
		*/
		static bool Move(const String& src, const String& dst);

		/**
		* @brief ファイルをコピーする
		* @param src コピー元ファイル名
		* @param dst コピー先ファイル名
		* @return 成否
		*/
		static bool Copy(const String& src, const String& dst, bool overWrite=false);
	};
}

#endif
