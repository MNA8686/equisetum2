/**
* @file DirectoryCompat.inl
* @brief ディレクトリ操作のターゲット依存部分実装クラス
* @author MNA
* @date 2017/04/13 新規作成
*/

#include "fs/Path.hpp"
#include "fs/File.hpp"
#include "fs/Directory.hpp"
#include <windows.h>
#include <winbase.h>
#include <memory>

namespace Equisetum2
{
	/**
	* ディレクトリ操作のターゲット依存部分実装クラス<br>
	* ここに入ってくるパスは絶対パスかつ正規化されていることを前提とし、<br>
	* それらのチェックは呼び出し側の責任である。
	*/
	class DirectoryCompat
	{
	public:

		static bool Create(const String& absPath)
		{
			return !!CreateDirectoryW((LPCWSTR)absPath.to_u16().c_str(), nullptr);
		}

		static bool Delete(const String& absPath, bool recursion = false)
		{
			return !!RemoveDirectoryW((LPCWSTR)absPath.to_u16().c_str());
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
				(attr & FILE_ATTRIBUTE_DIRECTORY) != 0)
			{
				ret = true;
			}

			return ret;
		}

		static bool EnumerateFiles(const String& absPath, const std::function<bool(const String&)> cb)
		{
			return Enumerate(absPath, cb, false);
		}

		static bool EnumerateDirectories(const String& absPath, const std::function<bool(const String&)> cb)
		{
			return Enumerate(absPath, cb, true);
		}

	private:

		static bool Enumerate(const String& absPath, const std::function<bool(const String&)> cb, bool isDirectory)
		{
			WIN32_FIND_DATAW findFileData;
			auto ret = false;

			auto handle = ::FindFirstFileW((LPCWSTR)(absPath + "*").to_u16().c_str(), &findFileData);
			if (handle != INVALID_HANDLE_VALUE)
			{
				auto handlePtr = std::shared_ptr<void>(handle, [](HANDLE handle) {
					::FindClose(handle);
				});

				do
				{
					String fileName((const char16_t*)findFileData.cFileName);	// UTF-16 -> UTF-8
					String enumratePath = absPath + fileName + (isDirectory ? Path::DirectorySeparatorChar : "");

					if (fileName != "." &&
						fileName != ".." &&
						(isDirectory ? Directory::Exists(enumratePath) : File::Exists(enumratePath)))
					{
						// 1件列挙ごとにコールバックを呼び出す
						if (auto cbRet = cb(enumratePath))
						{
							break;
						}
					}
				} while (::FindNextFileW(handlePtr.get(), &findFileData));

				ret = true;
			}

			return ret;
		}
	};
}

