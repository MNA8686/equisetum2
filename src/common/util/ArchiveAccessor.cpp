/**
* @file ArchiveAccessor.cpp
* @brief アーカイブアクセスクラス
* @author MNA
* @date 2017/08/08 新規作成
*/
#include "util/ArchiveAccessor.hpp"
#include "util/ArchivePacker.hpp"
#include "system/Logger.h"
#include "stream/PartialStream.hpp"
#include "stream/TextReader.h"
#include "util/SHA256.hpp"
#include "stream/CryptStream.hpp"

namespace Equisetum2
{
	std::shared_ptr<ArchiveAccessor> ArchiveAccessor::CreateFromStream(std::shared_ptr<IStream> inStream, const String& secretKey)
	{
		std::shared_ptr<ArchiveAccessor> inst;

		EQ_DURING
		{
			if (!inStream)
			{
				EQ_THROW(u8"有効なストリームではありません。");
			}

			if (!inStream->CanRead())
			{
				EQ_THROW(u8"リード属性が必要です。");
			}

			if (!inStream->CanSeek())
			{
				EQ_THROW(u8"シーク属性が必要です。");
			}

			auto inst_ = std::make_shared<ArchiveAccessor>(inStream, secretKey);

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

	ArchiveAccessor::ArchiveAccessor(std::shared_ptr<IStream> stream, const String& secretKey)
	{
		m_stream = stream;
		m_secretKey = secretKey;
		m_beginPos = m_stream->Position();
	}

	bool Equisetum2::ArchiveAccessor::Init()
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

			// テキストリーダー作成
			m_textReaderStream = TextReader::CreateFromStream(m_stream);
			if (!m_textReaderStream)
			{
				EQ_THROW(u8"テキストリーダーの作成に失敗しました。");
			}

			// ヘッダチェック
			auto optRead = m_textReaderStream->ReadLine();
			if (!optRead)
			{
				EQ_THROW(u8"ファイルヘッダの読み出しに失敗しました。");
			}
			if (*optRead != u8"Equ 2.0")
			{
				EQ_THROW(u8"ファイルヘッダが見つかりません。");
			}

			m_beginContainerPos = m_stream->Position();

			ret = true;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return ret;
	}

	bool ArchiveAccessor::EnumerateFiles(const std::function<bool(const ArchiveMeta&)> cb)
	{
		auto ret = false;

		EQ_DURING
		{
			if (!cb)
			{
				EQ_THROW(u8"コールバックが設定されていません。");
			}

			// ヘッダを読み飛ばした位置にシーク
			int64_t offset = m_stream->Seek(m_beginContainerPos, SeekOrigin::Begin);
			if (offset != m_beginContainerPos)
			{
				EQ_THROW(u8"シークに失敗しました。");
			}

			// ファイル群アクセス
			while (true)
			{
				// ファイル情報読み出し
				auto optMeta = m_textReaderStream->ReadLine();
				if (!optMeta)
				{
					EQ_THROW(u8"ファイル情報の読み出しに失敗しました。");
				}
				if ((*optMeta).substr(0, 5) == u8"%%EOF")
				{
					break;
				}

				ArchiveMeta meta;

				// ファイル情報はこのフォーマットで入っているのでバラす
				// "id=%s length=%lld crypt=%d"
				auto vSplit = (*optMeta).split();
				for (auto& elem : vSplit)
				{
					auto hash = elem.split("=");
					if (hash.size() != 2)
					{
						EQ_THROW(u8"ファイル情報が不正です。");
					}

					if (hash[0] == "id")
					{
						meta.id = hash[1];
					}
					else if (hash[0] == "length")
					{
						meta.length = std::stoll(hash[1]);
					}
					else if (hash[0] == "crypt")
					{
						meta.crypt = std::stoi(hash[1]);
					}
				}

				// 不正チェック
				if (meta.id.empty())
				{
					EQ_THROW(u8"idが不正です。");
				}
				if (meta.length < 0)
				{
					EQ_THROW(u8"lengthが不正です。");
				}
				if (meta.crypt < 0)
				{
					EQ_THROW(u8"cryptが不正です。");
				}

				// "stream" チェック
				auto optBegin = m_textReaderStream->ReadLine();
				if (!optBegin || (*optBegin) != "stream")
				{
					EQ_THROW(u8"streamの開始が見つかりません。");
				}

				// ファイルの中身の開始位置設定
				meta.offset = m_stream->Position() - m_beginPos;

				if (cb(meta))
				{
					break;
				}

				// ファイルの中身を読み飛ばす
				m_stream->Seek(meta.length, SeekOrigin::Current);

				// 改行読み飛ばし
				auto optCRLF = m_textReaderStream->ReadLine();
				if (!optCRLF || !(*optCRLF).empty())
				{
					EQ_THROW(u8"予期しないデータを検出しました。");
				}

				// "endstream" チェック
				auto optEnd = m_textReaderStream->ReadLine();
				if (!optEnd || (*optEnd) != "endstream")
				{
					EQ_THROW(u8"streamの終端が見つかりません。");
				}
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

	std::shared_ptr<IStream> ArchiveAccessor::SeekByArchiveMeta(const ArchiveMeta& meta)
	{
		auto ret = false;

		EQ_DURING
		{
			// ファイルの中身の先頭へ移動
			int64_t offset = m_stream->Seek(m_beginPos + meta.offset, SeekOrigin::Begin);
			if (offset != m_beginPos + meta.offset)
			{
				EQ_THROW(u8"シークに失敗しました。");
			}

			std::shared_ptr<IStream> stm;
			// ファイルの中身を単独のストリームとして扱えるようにする
			auto partialStream = PartialStream::CreateFromStream(m_stream, m_stream->Position(), meta.length);
			if (!partialStream)
			{
				EQ_THROW(u8"パーシャルストリームの作成に失敗しました。");
			}

			if (meta.crypt == 1)
			{
				// 暗号化を適用
				stm = CryptStream::CreateFromStream(partialStream, meta.id + m_secretKey);
			}
			else
			{
				// 暗号化されていない場合はパーシャルストリームをそのまま渡す
				stm = partialStream;
			}

			return stm;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return nullptr;

	}

	std::shared_ptr<IStream> ArchiveAccessor::FindByID(const String& id)
	{
		std::shared_ptr<IStream> ret;
		ArchiveMeta findMeta;

		EQ_DURING
		{
			if (id.empty())
			{
				EQ_THROW(u8"idが設定されていません。");
			}

			// ヘッダを読み飛ばした位置にシーク
			int64_t offset = m_stream->Seek(m_beginContainerPos, SeekOrigin::Begin);
			if (offset != m_beginContainerPos)
			{
				EQ_THROW(u8"シークに失敗しました。");
			}

			if(!EnumerateFiles([&findMeta, id](const ArchiveMeta& meta)->bool {
				if (meta.id == id)
				{
					findMeta = meta;
					return true;
				}
				return false;
			}))
			{
				EQ_THROW(u8"ファイルの列挙に失敗しました。");
			}

			if (!findMeta.id.empty())
			{
				ret = ArchiveAccessor::SeekByArchiveMeta(findMeta);
			}
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return ret;
	}

	bool ArchiveAccessor::HashCheck()
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

			const String eof = "%%EOF ";	// EOF識別子
			int64_t pos = -(static_cast<int64_t>(eof.size()));

			// ファイルの後ろからEOF識別子を探す
			while (true)
			{
				m_stream->Seek(pos, SeekOrigin::End);

				String find;
				for (size_t i = 0; i < eof.size(); i++)
				{
					find += m_textReaderStream->Read();
				}

				// EOF識別子？
				if (find == eof)
				{
					// EOF識別子発見
					break;
				}

				pos--;

				if (pos < -128)
				{
					EQ_THROW(u8"EOF識別子が見つかりません。");
				}
			}

			// EOF識別子の後に続くHMACを読み出す
			auto optHMAC1 = m_textReaderStream->ReadLine();
			if (!optHMAC1)
			{
				EQ_THROW(u8"HMACの読み出しに失敗しました。");
			}

			// HMAC算出用ストリームを作成する
			m_stream->Seek(pos, SeekOrigin::End);
			auto partial = PartialStream::CreateFromStream(m_stream, m_beginPos, m_stream->Position() - m_beginPos);
			if (!partial)
			{
				EQ_THROW(u8"パーシャルストリームの作成に失敗しました。");
			}

			// このファイルのHMACを算出する
			auto optHMAC2 = SHA256::HMAC(partial, m_secretKey);
			if (!optHMAC2)
			{
				EQ_THROW(u8"HMACの算出に失敗しました。");
			}

			// HMACが同一？
			if ((*optHMAC2).ToString() != (*optHMAC1))
			{
				EQ_THROW(u8"HMACが異なります。");
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

