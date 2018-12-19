/**
* @file ArchiveAccessor.hpp
* @brief アーカイブアクセスクラス
* @author MNA
* @date 2017/08/08 新規作成
*/
#if !defined(_EQARCHIVEACCESSOR_H_)
#define _EQARCHIVEACCESSOR_H_

#include "system/Exception.hpp"
#include "stream/IStream.h"
#include "stream/TextReader.h"

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
		int64_t offset = -1;	/// stream開始位置
	};

	/**
	* アーカイブアクセスクラス
	*/
	class ArchiveAccessor
	{
	public:
		/**
		* @brief アーカイブ入力インスタンスを作成する。<br>
		* @param inStream 入力ストリーム 読み出し属性、シーク属性が必要
		* @param secretKey 秘密鍵
		* @return アーカイブ入力インスタンス
		*/
		static std::shared_ptr<ArchiveAccessor> CreateFromStream(std::shared_ptr<IStream> inStream, const String& secretKey);

		/**
		* @brief メタ情報とストリームからアーカイブを読み出す。<br>
		* @brief 入力ストリームは正しいことが確定しているアーカイブデータ。<br>
		* @brief フォーマットのチェックは行わない。
		* @param inStream 入力ストリーム 読み出し属性、シーク属性が必要
		* @param meta シークしたいファイルのメタ情報
		* @param secretKey 秘密鍵
		* @return アーカイブ入力インスタンス
		*/
		static std::shared_ptr<IStream> QuickLoadFromStream(std::shared_ptr<IStream> inStream, const ArchiveMeta& meta, const String& secretKey);

		/**
		* @brief ディレクトリ内のファイルを列挙する 1件列挙するごとにコールバックが呼び出される
		* @param cb コールバック 引数に列挙したファイル名が入る。 trueを返すと列挙はそこで終了する。
		* @return 成否
		*/
		bool EnumerateFiles(const std::function<bool(const ArchiveMeta&)> cb);

		/**
		* @brief メタ情報で指定されたファイルをシークし、そのファイルのストリームを返す
		* @param meta シークしたいファイルのメタ情報
		* @return 発見されたIDのファイルストリーム。<br>
		* @return 取得したストリームはあたかも1個のファイルを扱っているかのように使用可能。<br>
		* @return 指定されたIDと一致するファイルが発見できなかった場合、nullptrが返る。
		*/
		std::shared_ptr<IStream> SeekByArchiveMeta(const ArchiveMeta& meta);

		/**
		* @brief 指定されたIDのファイルを探し、そのファイルのストリームを返す
		* @param id 探したいファイルのID
		* @return 発見されたIDのファイルストリーム。<br>
		* @return 取得したストリームはあたかも1個のファイルを扱っているかのように使用可能。<br>
		* @return 指定されたIDと一致するファイルが発見できなかった場合、nullptrが返る。
		*/
		std::shared_ptr<IStream> FindByID(const String& id);

		/**
		* @brief HMACをチェックする(ファイルのフォーマットが正しいかどうかまではチェックしない)
		* @return 成否
		*/
		bool HashCheck();

		ArchiveAccessor(std::shared_ptr<IStream> stream, const String& secretKey);
		virtual ~ArchiveAccessor() = default;

		/**
		* @brief アーカイブの作成を開始する
		* @return 成否
		*/
		bool Init();

	private:
		std::shared_ptr<IStream> m_stream;			/// アーカイブファイル入力ストリーム
		std::shared_ptr<TextReader> m_textReaderStream;		/// テキストファイル入力ストリーム
		int64_t m_beginPos = 0;		/// アーカイブ入力開始位置
		int64_t m_beginContainerPos = 0;		/// ファイル郡開始位置
		String m_secretKey;			/// 暗号化キー
	};
}

#endif
