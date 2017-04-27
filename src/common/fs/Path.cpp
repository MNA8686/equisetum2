/**
* @file Path.cpp
* @brief パス操作に関するクラス
* @author MNA
* @date 2017/04/12 新規作成
*/

#include "fs/Path.hpp"
#include "system/Exception.hpp"
#include "system/Logger.h"

#include "fs/PathCompat.inl"

namespace Equisetum2
{
	// セパレータの変換メソッドも必要
	// 原則、プログラム内では / を使用する かどうかは考える

	String Path::GetDirectoryName(const String& path)
	{
		return{};
	}

	String Path::GetExtension(const String& path)
	{
		return PathCompat::GetExtension(path);
	}

	String Path::GetFileName(const String& path)
	{
		return PathCompat::GetFileName(path);
	}

	String Path::GetFileNameWithoutExtension(const String& path)
	{
		return PathCompat::GetFileNameWithoutExtension(path);
	}

	String Path::GetFullPath(const String& path)
	{
		if (IsPathRooted(path))
		{
			return path;
		}
		else
		{
			// TODO: "." や ".." も考慮しないといけない
			return GetBasePath() + path;
		}
	}

	String Path::ChangeExtension(const String& path, const String& ext)
	{
		return PathCompat::ChangeExtension(path, ext);
	}

	bool Path::HasExtension(const String& path)
	{
		return PathCompat::HasExtension(path);
	}

	bool Path::IsPathRooted(const String& path)
	{
		return PathCompat::IsPathRooted(path);
	}

	String Path::GetBasePath()
	{
		return PathCompat::GetBasePath();
	}

	Optional<String> Path::GetTempPath(const String& org, const String& app)
	{
		Optional<String> optPath;

		EQ_DURING
		{
			if (org.size() == 0)
			{
				EQ_THROW("サークル名が入力されていません。");
			}

			if (app.size() == 0)
			{
				EQ_THROW("アプリケーション名が入力されていません。");
			}

			optPath = PathCompat::GetTempPath(org, app);
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER
			
		return optPath;
	}

	Optional<String> Path::GetCachePath(const String& org, const String& app)
	{
		Optional<String> optPath;

		EQ_DURING
		{
			if (org.size() == 0)
			{
				EQ_THROW("サークル名が入力されていません。");
			}

			if (app.size() == 0)
			{
				EQ_THROW("アプリケーション名が入力されていません。");
			}

			optPath = PathCompat::GetCachePath(org, app);
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return optPath;
	}

	Optional<String> Path::GetPrivateDocumentPath(const String& org, const String& app)
	{
		Optional<String> optPath;

		EQ_DURING
		{
			if (org.size() == 0)
			{
				EQ_THROW("サークル名が入力されていません。");
			}

			if (app.size() == 0)
			{
				EQ_THROW("アプリケーション名が入力されていません。");
			}

			optPath = PathCompat::GetPrivateDocumentPath(org, app);
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return optPath;
	}

	Optional<String> Path::GetPublicDocumentPath(const String& org, const String& app)
	{
		Optional<String> optPath;

		EQ_DURING
		{
			if (org.size() == 0)
			{
				EQ_THROW("サークル名が入力されていません。");
			}

			if (app.size() == 0)
			{
				EQ_THROW("アプリケーション名が入力されていません。");
			}

			optPath = PathCompat::GetPublicDocumentPath(org, app);
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return optPath;
	}

	const String Path::DirectorySeparatorChar = PathCompat::DirectorySeparatorChar;
}

