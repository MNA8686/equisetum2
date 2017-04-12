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
		static bool Create(const String& name);
		static bool Delete(const String& name);
		static bool Move(const String& src, const String& dst);
		static bool Exists(const String& name);
		static bool Copy(const String& src, const String& dst, bool overWrite=false);
	};
}

#endif
