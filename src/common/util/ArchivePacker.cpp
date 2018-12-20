/**
* @file ArchivePacker.cpp
* @brief アーカイブパッカークラス
* @author MNA
* @date 2017/08/08 新規作成
*/
#include "util/ArchivePacker.hpp"
#include "system/Logger.h"
#include "util/SHA256.hpp"
#include "util/Regex.hpp"
#include "stream/PartialStream.hpp"
#include "stream/CryptStream.hpp"

namespace Equisetum2
{
	std::shared_ptr<ArchivePacker> ArchivePacker::CreateFromStream(std::shared_ptr<IStream> outStream, const String& secretKey)
	{
		std::shared_ptr<ArchivePacker> inst;

		EQ_DURING
		{
			if (!outStream)
			{
				EQ_THROW(u8"有効なストリームではありません。");
			}

			if (!outStream->CanRead())
			{
				EQ_THROW(u8"リード属性が必要です。");
			}

			if (!outStream->CanWrite())
			{
				EQ_THROW(u8"ライト属性が必要です。");
			}

			if (!outStream->CanSeek())
			{
				EQ_THROW(u8"シーク属性が必要です。");
			}

			auto inst_ = std::make_shared<ArchivePacker>(outStream, secretKey);

			if (!inst_)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			if (!inst_->Init())
			{
				EQ_THROW(u8"初期化に失敗しました。");
			}

			inst = inst_;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return inst;
	}

	ArchivePacker::ArchivePacker(std::shared_ptr<IStream> stream, const String& secretKey)
	{
		m_stream = stream;
		m_secretKey = secretKey;
		m_beginPos = m_stream->Position();
	}

	bool ArchivePacker::Init()
	{
		auto ret = false;

		EQ_DURING
		{
			// ストリームの先頭へ移動
			int64_t offset = m_stream->Seek(m_beginPos, SeekOrigin::Begin);
			if (offset != m_beginPos)
			{
				EQ_THROW(u8"シークに失敗しました。");
			}

			// テキストライター作成
			m_textWriterStream = TextWriter::CreateFromStream(m_stream);
			if (!m_textWriterStream)
			{
				EQ_THROW(u8"テキストライターの作成に失敗しました。");
			}

			// ヘッダを書き込む
			if (!m_textWriterStream->WriteLine(u8"Equ 2.0"))
			{
				EQ_THROW(u8"ファイルヘッダの書き込みに失敗しました。");
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

	bool ArchivePacker::SetCrypt(uint32_t crypt)
	{
		m_isCrypt = crypt;
		return true;
	}

	bool ArchivePacker::Push(const String& id, std::shared_ptr<IStream> inStream)
	{
		auto ret = false;

		EQ_DURING
		{
			if (!inStream)
			{
				EQ_THROW(u8"入力ストリームは有効なストリームではありません。");
			}

			if (!inStream->CanRead())
			{
				EQ_THROW(u8"入力ストリームにはリード属性が必要です。");
			}

			if (id.empty())
			{
				EQ_THROW(u8"idが設定されていません。");
			}

			// idに使えない文字が無いかチェック
			auto search = Regex::Search(id, u8"[^0-9a-zA-Z._/\\-|]+");
			if (search)
			{
				EQ_THROW(u8"idに使える文字は [0-9a-zA-Z._/\\-|] のみです。");
			}

			// ファイル情報を書き込む
			if (!m_textWriterStream->WriteLine(String::Sprintf(u8"id=%s length=%lld crypt=%d", id.c_str(), inStream->Length(), m_isCrypt)))
			{
				EQ_THROW(u8"ファイル情報の書き込みに失敗しました。");
			}

			std::shared_ptr<IStream> cryptStream;
			if (m_isCrypt == 1)
			{
				if (m_secretKey.empty())
				{
					EQ_THROW(u8"キーが設定されていません。");
				}

				// 暗号化を適用
				cryptStream = CryptStream::CreateFromStream(inStream, id + m_secretKey);
			}
			else
			{
				// 暗号化不要の場合は入力されたストリームをそのまま使用する
				cryptStream = inStream;
			}
			if (!cryptStream)
			{
				EQ_THROW(u8"ストリームの作成に失敗しました。");
			}

			// ファイルを書き込む
			if (!m_textWriterStream->WriteLine(u8"stream") ||
				!cryptStream->CopyTo(m_stream) ||
				!m_textWriterStream->WriteLine(u8"") ||
				!m_textWriterStream->WriteLine(u8"endstream"))
			{
				EQ_THROW(u8"ファイルの書き込みに失敗しました。");
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

	bool ArchivePacker::Finalize()
	{
		auto ret = false;

		EQ_DURING
		{
			// HMAC算出範囲のパーシャルストリームを作成する
			auto partial = PartialStream::CreateFromStream(m_stream, m_beginPos, m_stream->Position() - m_beginPos);
			if (!partial)
			{
				EQ_THROW(u8"パーシャルストリームの作成に失敗しました。");
			}

			// HMAC算出
			auto hmac = SHA256::HMAC(partial, m_secretKey);
			if (!hmac)
			{
				EQ_THROW(u8"HMACの算出に失敗しました。");
			}

			// フッター書き込み
			if (!m_textWriterStream->WriteLine(u8"%%EOF " + (*hmac).ToString()))
			{
				EQ_THROW(u8"ファイルフッターの書き込みに失敗しました。");
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
}

