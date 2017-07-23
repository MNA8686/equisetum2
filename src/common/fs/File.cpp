/**
* @file File.cpp
* @brief ファイル操作に関するクラス
* @author MNA
* @date 2017/04/12 新規作成
*/

#include "fs/Path.hpp"
#include "fs/File.hpp"
#include "fs/Directory.hpp"
#include "system/Exception.hpp"
#include "system/Logger.h"
#include "stream/FileStream.h"

#include "fs/FileCompat.inl"

namespace Equisetum2
{
	bool File::Create(const String& path)
	{
		auto ret = false;

		EQ_DURING
		{
			if (File::Exists(path))
			{
				EQ_THROW(String::Sprintf(u8"すでにファイル[%s]は存在します。", path.c_str()));
			}

			if (Directory::Exists(path))
			{
				EQ_THROW(String::Sprintf(u8"すでにディレクトリ[%s]が存在します。", path.c_str()));
			}

			auto inst = FileStream::CreateFromPath(Path::GetFullPath(path), FileStream::Method::Write);
			if (!inst)
			{
				EQ_THROW(String::Sprintf(u8"[%s]の作成に失敗しました。", path.c_str()));
			}

			ret = true;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER
			
		return ret;
	}

	bool File::Delete(const String& path)
	{
		auto ret = false;

		EQ_DURING
		{
			if (!File::Exists(path))
			{
				EQ_THROW(String::Sprintf(u8"削除ファイル[%s]は存在しません。", path.c_str()));
			}

			ret = FileCompat::Delete(Path::GetFullPath(path));
			if (!ret)
			{
				EQ_THROW(String::Sprintf(u8"[%s]の削除に失敗しました。", path.c_str()));
			}
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return ret;
	}

	bool File::Move(const String& src, const String& dst)
	{
		auto ret = false;

		EQ_DURING
		{
			if (!File::Exists(src))
			{
				EQ_THROW(String::Sprintf(u8"移動元[%s]は存在しません。", src.c_str()));
			}

			if (File::Exists(dst))
			{
				EQ_THROW(String::Sprintf(u8"移動先[%s]はすでに同名のファイルが存在します。", dst.c_str()));
			}

			if (Directory::Exists(dst))
			{
				EQ_THROW(String::Sprintf(u8"移動先[%s]はすでに同名のディレクトリが存在します。", dst.c_str()));
			}

			ret = FileCompat::Move(Path::GetFullPath(src), Path::GetFullPath(dst));
			if (!ret)
			{
				EQ_THROW(String::Sprintf(u8"[%s]から[%s]への移動に失敗しました。", src.c_str(), dst.c_str()));
			}
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return ret;
	}

	bool File::Exists(const String& path)
	{
		return FileCompat::Exists(Path::GetFullPath(path));
	}

	bool File::Copy(const String& src, const String& dst, bool overWrite)
	{
		auto ret = false;

		EQ_DURING
		{
			if (!File::Exists(src))
			{
				EQ_THROW(String::Sprintf(u8"コピー元[%s]は存在しません。", src.c_str()));
			}

			if (Directory::Exists(dst))
			{
				EQ_THROW(String::Sprintf(u8"コピー先[%s]はすでに同名のディレクトリが存在します。", dst.c_str()));
			}

			if (!overWrite && File::Exists(dst))
			{
				EQ_THROW(String::Sprintf(u8"コピー先[%s]はすでに同名のファイルが存在します。上書きするにはoverWriteにtrueを設定してください。", dst.c_str()));
			}

			ret = FileCompat::Copy(Path::GetFullPath(src), Path::GetFullPath(dst), overWrite);
			if (!ret)
			{
				EQ_THROW(String::Sprintf(u8"[%s]から[%s]へのコピーに失敗しました。", src.c_str(), dst.c_str()));
			}
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return ret;
	}
}

