/**
* @file Directory.cpp
* @brief ディレクトリ操作に関するクラス
* @author MNA
* @date 2017/04/13 新規作成
*/

#include "fs/Directory.hpp"
#include "system/Exception.hpp"
#include "system/Logger.h"
#include <iterator>

#include "fs/DirectoryCompat.inl"

namespace Equisetum2
{
	bool Directory::Create(const String& path)
	{
		auto ret = false;

		EQ_DURING
		{
			if (File::Exists(path))
			{
				EQ_THROW(String::Sprintf(u8"すでにファイル[%s]が存在します。", path.c_str()));
			}

			if (Directory::Exists(path))
			{
				EQ_THROW(String::Sprintf(u8"すでにディレクトリ[%s]は存在します。", path.c_str()));
			}

			ret = DirectoryCompat::Create(Path::GetFullPath(path));
			if (!ret)
			{
				EQ_THROW(String::Sprintf(u8"[%s]の作成に失敗しました。", path.c_str()));
			}

		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return ret;
	}

	bool Directory::Delete(const String& path, bool recursion)
	{
		// TODO: recursion実装する？

		auto ret = false;

		EQ_DURING
		{
			if (!Directory::Exists(path))
			{
				EQ_THROW(String::Sprintf(u8"削除ファイル[%s]は存在しません。", path.c_str()));
			}

			ret = DirectoryCompat::Delete(Path::GetFullPath(path));
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

	bool Directory::Move(const String& src, const String& dst)
	{
		auto ret = false;

		EQ_DURING
		{
			if (!Directory::Exists(src))
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

			ret = DirectoryCompat::Move(Path::GetFullPath(src), Path::GetFullPath(dst));
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

	bool Directory::Exists(const String& path)
	{
		return DirectoryCompat::Exists(Path::GetFullPath(path));
	}

	bool Directory::EnumerateFiles(const String& path, const std::function<bool(const String&)> cb)
	{
		auto ret = false;

		EQ_DURING
		{
			std::vector<String> v;
			auto hasSeparate = false;

			if (path.size() == 0)
			{
				EQ_THROW(u8"パスが入力されていません。");
			}

			if (!Directory::Exists(path))
			{
				EQ_THROW(String::Sprintf(u8"ディレクトリ[%s]は存在しません。", path.c_str()));
			}

			// ディレクトリであることがここで確定する
			// パスの終端が\\で無い場合、問題が起きるので付け足す処理の準備
			if (path[path.size() - 1] == '\\')
			{
				hasSeparate = true;
			}

			auto absPath = Path::GetFullPath(path + (hasSeparate ? "" : Path::DirectorySeparatorChar));
			if (!DirectoryCompat::EnumerateFiles(absPath, cb))
			{
				EQ_THROW(String::Sprintf(u8"ディレクトリ[%s]内の列挙に失敗しました。", path.c_str()));
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

	Optional<std::vector<String>> Directory::GetFiles(const String& path)
	{
		Optional<std::vector<String>> opt;

		EQ_DURING
		{
			std::vector<String> v;

			if (!EnumerateFiles(path, [&v](const String& name)->bool {
				v.push_back(name);
				return false;
			}))
			{
				EQ_THROW(String::Sprintf(u8"ディレクトリ[%s]内の列挙に失敗しました。", path.c_str()));
			}

			opt = v;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return opt;
	}

	bool Directory::EnumerateDirectories(const String& path, const std::function<bool(const String&)> cb)
	{
		auto ret = false;

		EQ_DURING
		{
			auto hasSeparate = false;

			if (path.size() == 0)
			{
				EQ_THROW(u8"パスが入力されていません。");
			}

			if (!Directory::Exists(path))
			{
				EQ_THROW(String::Sprintf(u8"ディレクトリ[%s]は存在しません。", path.c_str()));
			}

			// ディレクトリであることがここで確定する
			// パスの終端が\\で無い場合、問題が起きるので付け足す処理の準備
			if (path[path.size() - 1] == '\\')
			{
				hasSeparate = true;
			}

			auto absPath = Path::GetFullPath(path + (hasSeparate ? "" : Path::DirectorySeparatorChar));
			if (!DirectoryCompat::EnumerateDirectories(absPath, cb))
			{
				EQ_THROW(String::Sprintf(u8"ディレクトリ[%s]内の列挙に失敗しました。", path.c_str()));
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

	Optional<std::vector<String>> Directory::GetDirectories(const String& path, bool recursion)
	{
		Optional<std::vector<String>> opt;

		EQ_DURING
		{
			std::vector<String> v;

			if (!EnumerateDirectories(path, [&v, recursion](const String& name)->bool {
				v.push_back(name);

				if (recursion)
				{
					// 再帰呼び出しを行う
					Optional<std::vector<String>> optRecDir = GetDirectories(name, true);
					if (optRecDir)
					{
						// ディレクトリ一覧を連結する
						auto& recDir = *optRecDir;
						v.reserve(v.size() + recDir.size()); 
						std::copy(recDir.begin(), recDir.end(), std::back_inserter(v));
					}
				}

				return false;
			}))
			{
				EQ_THROW(String::Sprintf(u8"ディレクトリ[%s]内の列挙に失敗しました。", path.c_str()));
			}

			opt = v;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return opt;
	}
}

