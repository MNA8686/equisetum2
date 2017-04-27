/**
* @file File.cpp
* @brief ファイル操作に関するクラス
* @author MNA
* @date 2017/04/12 新規作成
*/

#include "fs/File.hpp"
#include <windows.h>
#include <winbase.h>

namespace Equisetum2
{
	/**
	* ファイル操作のターゲット依存部分実装クラス<br>
	* ここに入ってくるパスは絶対パスかつ正規化されていることを前提とし、<br>
	* それらのチェックは呼び出し側の責任である。
	*/
	class FileCompat
	{
	public:

		static bool Delete(const String& absPath)
		{
			return !!::DeleteFileW((LPCWSTR)absPath.to_u16().c_str());
		}

		static bool Move(const String& absSrcPath, const String& absDstPath)
		{
			return !!::MoveFileW((LPCWSTR)absSrcPath.to_u16().c_str(), (LPCWSTR)absDstPath.to_u16().c_str());
		}

		static bool Exists(const String& absPath)
		{
			auto attr = ::GetFileAttributesW((LPCWSTR)absPath.to_u16().c_str());
			auto ret = false;

			if (attr != -1 &&
				(attr & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				ret = true;
			}

			return ret;
		}

		static bool Copy(const String& absSrcPath, const String& absDstPath, bool overWrite)
		{
			return !!CopyFileW((LPCWSTR)absSrcPath.to_u16().c_str(), (LPCWSTR)absDstPath.to_u16().c_str(), !overWrite);
		}
	};
}

