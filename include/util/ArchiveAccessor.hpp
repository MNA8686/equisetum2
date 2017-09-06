﻿/**
* @file ArchiveAccessor.hpp
* @brief アーカイブアクセスクラス
* @author MNA
* @date 2017/08/08 新規作成
*/
#if !defined(_EQARCHIVEACCESSOR_H_)
#define _EQARCHIVEACCESSOR_H_

#include "system/Exception.hpp"
#include "stream/IStream.h"

namespace Equisetum2
{
	/**
	* アーカイブファイル情報構造体
	*/
	struct ArchiveMeta
	{
		String id;		/// リソース識別ID
		int64_t length = -1;	/// リソースサイズ
		uint32_t crypt = -1;	/// 暗号化種別
	};

	/**
	* アーカイブアクセスクラス
	*/
	class ArchiveAccessor
	{
	public:
		/**
		* @brief ディレクトリ内のファイルを列挙する 1件列挙するごとにコールバックが呼び出される
		* @param path 対象ディレクトリ名
		* @param cb コールバック 引数に列挙したファイル名が入る。 trueを返すと列挙はそこで終了する。
		* @return 成否
		*/
		static bool EnumerateFiles(std::shared_ptr<IStream> stream, const std::function<bool(const ArchiveMeta&, std::shared_ptr<IStream>)> cb);

		/**
		* @brief 指定されたIDのファイルを探し、そのファイルのストリームを返す
		* @param stream 入力ストリーム
		* @param id 探したいファイルのID
		* @return 発見されたIDのファイルストリーム。<br>
		* @return 取得したストリームはあたかも1個のファイルを扱っているかのように使用可能。<br>
		* @return 指定されたIDと一致するファイルが発見できなかった場合、nullptrが返る。
		*/
		static std::shared_ptr<IStream> FindFromStream(std::shared_ptr<IStream> stream, const String& id);

		/**
		* @brief HMACをチェックする(ファイルのフォーマットが正しいかどうかまではチェックしない)
		* @param stream 入力ストリーム
		* @return 成否
		*/
		static bool CheckFromStream(std::shared_ptr<IStream> stream);
	};
}

#endif
