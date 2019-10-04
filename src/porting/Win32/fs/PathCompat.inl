/**
* @file PathCompat.inl
* @brief パス操作のターゲット依存部分実装クラス
* @author MNA
* @date 2017/04/13 新規作成
*/

#include "fs/Path.hpp"
#include <windows.h>
//#include <winbase.h>
//#include <memory>
#include <shlobj.h>
#include "fs/Directory.hpp"

#undef GetTempPath

namespace Equisetum2
{
	/**
	* パス操作のターゲット依存部分実装クラス
	*/	
	class PathCompat
	{
	public:

		static String GetDirectoryName(const String& path)
		{

		}

		static String GetExtension(const String& path)
		{
			auto info = SplitPath(path);
			return info.ext;
		}

		static String GetFileName(const String& path)
		{
			auto info = SplitPath(path);
			return info.fname + info.ext;
		}

		static String GetFileNameWithoutExtension(const String& path)
		{
			auto info = SplitPath(path);
			return info.fname;
		}

		static String GetFullPath(const String& path)
		{

		}

		static String GetParent(const String& path)
		{

		}

		static String ChangeExtension(const String& path, const String& ext)
		{
			auto info = SplitPath(path);
			return info.drive + info.dir + info.fname + ext;
		}

		static bool HasExtension(const String& path)
		{
			auto info = SplitPath(path);
			return info.ext.size() > 0;
		}

		static bool IsPathRooted(const String& path)
		{
			auto info = SplitPath(path);
			return info.drive.size() > 0 || path.find("\\\\") == 0;
		}

		static String GetBasePath()
		{
			String basePath;

			// 実行ファイルの場所を取得する
			std::vector<wchar_t> szPath(128);

			while (true)
			{
				auto len = ::GetModuleFileNameW(NULL, &szPath[0], szPath.size());
				if (len != szPath.size())
				{
					String modulePath((const char16_t*)&szPath[0]);

					auto info = SplitPath(modulePath);
					basePath = info.drive + info.dir;
					break;
				}

				szPath.resize(szPath.size() * 2);
			}

			return basePath;
		}

		static Optional<String> GetTempPath(const String& org, const String& app)
		{
			Optional<String> optPath;

			EQ_DURING
			{
				// 実行ファイルの場所を取得する
				std::vector<wchar_t> szPath(128);

				while (true)
				{
					auto len = ::GetTempPathW(szPath.size(), &szPath[0]);
					if (len == 0)
					{
						EQ_THROW(u8"テンポラリディレクトリの取得に失敗しました。");
					}

					if (len != szPath.size())
					{
						break;
					}

					szPath.resize(szPath.size() * 2);
				}

				String pathUTF8((const char16_t*)&szPath[0]);	// UTF-16 -> UTF-8

				if (pathUTF8[pathUTF8.size() - 1] != '\\')
				{
					pathUTF8 += DirectorySeparatorChar;
				}

				pathUTF8 += org + DirectorySeparatorChar;
				if (!Directory::Exists(pathUTF8))
				{
					if (!Directory::Create(pathUTF8))
					{
						EQ_THROW(String::Sprintf(u8"[%s]の作成に失敗しました。", pathUTF8.c_str()));
					}
				}

				pathUTF8 += app + DirectorySeparatorChar;
				if (!Directory::Exists(pathUTF8))
				{
					if (!Directory::Create(pathUTF8))
					{
						EQ_THROW(String::Sprintf(u8"[%s]の作成に失敗しました。", pathUTF8.c_str()));
					}
				}

				// 例: "C:\\Users\\(user)\\AppData\\Local\\Temp\\(org)\\(app)\\" on Win10
				optPath = pathUTF8;
			}
			EQ_HANDLER
			{
				Logger::OutputError(EQ_GET_HANDLER().what());
			}
			EQ_END_HANDLER

			return optPath;
		}

		static Optional<String> GetCachePath(const String& org, const String& app)
		{
			Optional<String> optPath;

			EQ_DURING
			{
				wchar_t path[MAX_PATH];

				auto result = ::SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, NULL, NULL, path);
				if (result != S_OK)
				{
					EQ_THROW(u8"キャッシュディレクトリの取得に失敗しました。");
				}

				String pathUTF8((const char16_t*)path);	// UTF-16 -> UTF-8

				if (pathUTF8[pathUTF8.size() - 1] != '\\')
				{
					pathUTF8 += DirectorySeparatorChar;
				}

				pathUTF8 += org + DirectorySeparatorChar;
				if (!Directory::Exists(pathUTF8))
				{
					if (!Directory::Create(pathUTF8))
					{
						EQ_THROW(String::Sprintf(u8"[%s]の作成に失敗しました。", pathUTF8.c_str()));
					}
				}

				pathUTF8 += app + DirectorySeparatorChar;
				if (!Directory::Exists(pathUTF8))
				{
					if (!Directory::Create(pathUTF8))
					{
						EQ_THROW(String::Sprintf(u8"[%s]の作成に失敗しました。", pathUTF8.c_str()));
					}
				}

				pathUTF8 += "cache" + DirectorySeparatorChar;
				if (!Directory::Exists(pathUTF8))
				{
					if (!Directory::Create(pathUTF8))
					{
						EQ_THROW(String::Sprintf(u8"[%s]の作成に失敗しました。", pathUTF8.c_str()));
					}
				}

				// 例: "C:\\Users\\(user)\\AppData\\Local\\(org)\\(app)\\cache\\" on Win10
				optPath = pathUTF8;
			}
			EQ_HANDLER
			{
				Logger::OutputError(EQ_GET_HANDLER().what());
			}
			EQ_END_HANDLER

			return optPath;
		}

		static Optional<String> GetPrivateDocumentPath(const String& org, const String& app)
		{
			Optional<String> optPath;

			EQ_DURING
			{
				wchar_t path[MAX_PATH];

				auto result = ::SHGetFolderPathW(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, NULL, path);
				if (result != S_OK)
				{
					EQ_THROW(u8"プライベートドキュメントディレクトリの取得に失敗しました。");
				}

				String pathUTF8((const char16_t*)path);	// UTF-16 -> UTF-8

				if (pathUTF8[pathUTF8.size() - 1] != '\\')
				{
					pathUTF8 += DirectorySeparatorChar;
				}

				pathUTF8 += org + DirectorySeparatorChar;
				if (!Directory::Exists(pathUTF8))
				{
					if (!Directory::Create(pathUTF8))
					{
						EQ_THROW(String::Sprintf(u8"[%s]の作成に失敗しました。", pathUTF8.c_str()));
					}
				}

				pathUTF8 += app + DirectorySeparatorChar;
				if (!Directory::Exists(pathUTF8))
				{
					if (!Directory::Create(pathUTF8))
					{
						EQ_THROW(String::Sprintf(u8"[%s]の作成に失敗しました。", pathUTF8.c_str()));
					}
				}

				// 例: "C:\\Users\\(user)\\AppData\\Roaming\\(org)\\(app)\\" on Win10
				optPath = pathUTF8;
			}
			EQ_HANDLER
			{
				Logger::OutputError(EQ_GET_HANDLER().what());
			}
			EQ_END_HANDLER

			return optPath;
		}

		static Optional<String> GetPublicDocumentPath(const String& org, const String& app)
		{
			Optional<String> optPath;

			EQ_DURING
			{
				wchar_t path[MAX_PATH];

				auto result = ::SHGetFolderPathW(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, NULL, NULL, path);
				if (result != S_OK)
				{
					EQ_THROW(u8"パブリックドキュメントディレクトリの取得に失敗しました。");
				}

				String pathUTF8((const char16_t*)path);	// UTF-16 -> UTF-8

				if (pathUTF8[pathUTF8.size() - 1] != '\\')
				{
					pathUTF8 += DirectorySeparatorChar;
				}

				pathUTF8 += org + DirectorySeparatorChar;
				if (!Directory::Exists(pathUTF8))
				{
					if (!Directory::Create(pathUTF8))
					{
						EQ_THROW(String::Sprintf(u8"[%s]の作成に失敗しました。", pathUTF8.c_str()));
					}
				}

				pathUTF8 += app + DirectorySeparatorChar;
				if (!Directory::Exists(pathUTF8))
				{
					if (!Directory::Create(pathUTF8))
					{
						EQ_THROW(String::Sprintf(u8"[%s]の作成に失敗しました。", pathUTF8.c_str()));
					}
				}

				// 例: "C:\\Users\\(user)\\Documents\\(org)\\(app)\\" on Win10
				optPath = pathUTF8;
			}
			EQ_HANDLER
			{
				Logger::OutputError(EQ_GET_HANDLER().what());
			}
			EQ_END_HANDLER

			return optPath;
		}

		static const String DirectorySeparatorChar;

	private:

		class PathInfo
		{
		public:
			String drive;
			String dir;
			String fname;
			String ext;
		};

		static PathInfo SplitPath(const String& path)
		{
			wchar_t drive[_MAX_DRIVE + 1] = {};
			wchar_t dir[_MAX_DIR + 1] = {};
			wchar_t fname[_MAX_FNAME + 1] = {};
			wchar_t ext[_MAX_EXT + 1] = {};
				
			_wsplitpath_s(
				(wchar_t const*)path.to_u16().c_str(),
				drive, _MAX_DRIVE,
				dir, _MAX_FNAME,
				fname, _MAX_FNAME,
				ext, _MAX_EXT
			);

			return{ (const char16_t*)drive, (const char16_t*)dir, (const char16_t*)fname, (const char16_t*)ext };
		}
	};

	const String PathCompat::DirectorySeparatorChar = "\\";
}

