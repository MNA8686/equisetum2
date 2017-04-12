/**
* @file Path.hpp
* @brief パス操作に関するクラス
* @author MNA
* @date 2017/04/12 新規作成
*/
#if !defined(_EQPATH_H_)
#define _EQPATH_H_

#include "type/String.h"
#include "type/Optional.h"

namespace Equisetum2
{
	/**
	* パス操作に関するクラス
	*/
	class Path
	{
	public:

		static String GetDirectoryName(const String& path);
		static String GetExtension(const String& path);
		static String GetFileName(const String& path);
		static String GetFileNameWithoutExtension(const String& path);
		static String GetFullPath(const String& path);
		static String GetParent(const String& path);
		static bool ChangeExtension(const String& path, const String& ext);
		static bool HasExtension(const String& path);

		static String GetTempPath();
		static String GetCachePath();
		static String GetPrivateDocumentPath();
		static String GetPublicDocumentPath();

		static const String DirectorySeparatorChar;// = "\\";
	};
}

#endif
