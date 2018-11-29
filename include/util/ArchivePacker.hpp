/**
* @file ArchivePacker.hpp
* @brief アーカイブパッカークラス
* @author MNA
* @date 2017/08/08 新規作成
*/
#if !defined(_EQARCHIVEPACKER_H_)
#define _EQARCHIVEPACKER_H_

#include "system/Exception.hpp"
#include "stream/TextWriter.h"

namespace Equisetum2
{
	/*
	・アーカイブのフォーマット

		Equ (アーカイブのバージョン)\r\n
		id=path length=xxxx crypt=1\r\n
		stream\r\n
		xxxxxxxxxxxxxxxxxxxxx\r\n
		endstream\r\n
		id=path length=xxxx crypt=1\r\n
		stream\r\n
		xxxxxxxxxxxxxxxxxxxxx\r\n
		endstream\r\n
		...
		%%EOF (HMAC)
	*/

	/**
	* アーカイブパッカークラス
	*/
	class ArchivePacker
	{
	public:
		/**
		* @brief アーカイブ出力インスタンスを作成する。<br>
		* @brief このメソッドを実行すると、出力ストリームにファイルヘッダが書き込まれる。
		* @param outStream 出力ストリーム 書き込み属性、読み出し属性、シーク属性が必要
		* @param secretKey 秘密鍵
		* @return アーカイブ出力インスタンス
		*/
		static std::shared_ptr<ArchivePacker> CreateFromStream(std::shared_ptr<IStream> outStream, const String& secretKey);

		/**
		* @brief ファイルの暗号化方式を設定する
		* @param crypt 0:暗号化なし
		* @return 成否
		*/
		bool SetCrypt(uint32_t crypt);

		/**
		* @brief ファイルをひとつ書き込む SetCrypt()で設定した暗号化方式が適用される
		* @param id ファイルのID 使用可能な文字は [0-9a-zA-Z._/] のみ。文字数は特に規定しない。
		* @param inStream アーカイブに書き込みたいファイルのストリーム
		* @return 成否
		*/
		bool Push(const String& id, std::shared_ptr<IStream> inStream);

		/**
		* @brief ファイルのフッター(EOF識別子とHMAC)を書き込む
		* @return 成否
		*/
		bool Finalize();

		ArchivePacker(std::shared_ptr<IStream> stream, const String& secretKey);
		virtual ~ArchivePacker() = default;

		/**
		* @brief アーカイブの作成を開始する
		* @return 成否
		*/
		bool Init();

	private:
		std::shared_ptr<IStream> m_stream;		/// アーカイブファイル出力ストリーム
		std::shared_ptr<TextWriter> m_textWriterStream;		/// テキストファイル出力ストリーム
		int64_t m_beginPos = 0;		/// アーカイブ出力開始位置
		uint32_t m_isCrypt = 0;		/// 暗号化種別
		String m_secretKey;			/// 暗号化キー
	};
}

#endif
