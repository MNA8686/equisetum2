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
	bool ArchiveAccessor::EnumerateFiles(std::shared_ptr<IStream> stream, const std::function<bool(const ArchiveMeta&, std::shared_ptr<IStream>)> cb, const String& secretKey)
	{
		auto ret = false;

		EQ_DURING
		{
			if (!stream)
			{
				EQ_THROW(u8"有効なストリームではありません。");
			}

			if (!stream->CanRead())
			{
				EQ_THROW(u8"リード属性が必要です。");
			}

			if (!stream->CanSeek())
			{
				EQ_THROW(u8"シーク属性が必要です。");
			}

			if (!cb)
			{
				EQ_THROW(u8"コールバックが設定されていません。");
			}

			auto textReader = TextReader::CreateFromStream(stream);
			if (!textReader)
			{
				EQ_THROW(u8"テキストリーダーの作成に失敗しました。");
			}

			// ヘッダチェック
			{
				auto optRead = textReader->ReadLine();
				if (!optRead)
				{
					EQ_THROW(u8"ファイルヘッダの読み出しに失敗しました。");
				}
				if (*optRead != u8"Equ 2.0")
				{
					EQ_THROW(u8"ファイルヘッダが見つかりません。");
				}
			}

			while (true)
			{
				// ファイル情報読み出し
				auto optMeta = textReader->ReadLine();
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
				auto optBegin = textReader->ReadLine();
				if (!optBegin || (*optBegin) != "stream")
				{
					EQ_THROW(u8"streamの開始が見つかりません。");
				}

				std::shared_ptr<IStream> stm;
				// ファイルの中身を単独のストリームとして扱えるようにする
				auto partialStream = PartialStream::CreateFromStream(stream, stream->Position(), meta.length);
				if (!partialStream)
				{
					EQ_THROW(u8"パーシャルストリームの作成に失敗しました。");
				}

				if (meta.crypt == 1)
				{
					// 暗号化を適用
					stm = CryptStream::CreateFromStream(partialStream, meta.id + secretKey);
				}
				else
				{
					// 暗号化されていない場合はパーシャルストリームをそのまま渡す
					stm = partialStream;
				}

				if (cb(meta, stm))
				{
					break;
				}

				// ファイルの中身を読み飛ばす
				stream->Seek(meta.length, SeekOrigin::Current);

				// 改行読み飛ばし
				auto optCRLF = textReader->ReadLine();
				if (!optCRLF || !(*optCRLF).empty())
				{
					EQ_THROW(u8"予期しないデータを検出しました。");
				}

				// "endstream" チェック
				auto optEnd = textReader->ReadLine();
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

	std::shared_ptr<IStream> ArchiveAccessor::FindFromStream(std::shared_ptr<IStream> stream, const String& id, const String& secretKey)
	{
		std::shared_ptr<IStream> ret;

		EQ_DURING
		{
			if (id.empty())
			{
				EQ_THROW(u8"idが設定されていません。");
			}

			if(!EnumerateFiles(stream, [&ret, id](const ArchiveMeta& meta, std::shared_ptr<IStream> stream)->bool {
				if (meta.id == id)
				{
					ret = stream;
				}
				return !!ret;
			}, secretKey))
			{
				EQ_THROW(u8"ファイルの列挙に失敗しました。");
			}
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return ret;
	}

	bool ArchiveAccessor::CheckFromStream(std::shared_ptr<IStream> stream, const String& secretKey)
	{
		auto ret = false;

		EQ_DURING
		{
			if (!stream)
			{
				EQ_THROW(u8"有効なストリームではありません。");
			}

			if (!stream->CanRead())
			{
				EQ_THROW(u8"リード属性が必要です。");
			}

			if (!stream->CanSeek())
			{
				EQ_THROW(u8"シーク属性が必要です。");
			}

			auto textReader = TextReader::CreateFromStream(stream);
			if (!textReader)
			{
				EQ_THROW(u8"テキストリーダーの作成に失敗しました。");
			}

			// ストリーム開始位置を保存
			auto beginPos = stream->Position();

			const String eof = "%%EOF ";	// EOF識別子
			int64_t pos = -(static_cast<int64_t>(eof.size()));

			// ファイルの後ろからEOF識別子を探す
			while (true)
			{
				stream->Seek(pos, SeekOrigin::End);

				String find;
				for (size_t i = 0; i < eof.size(); i++)
				{
					find += textReader->Read();
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
			auto optHMAC1 = textReader->ReadLine();
			if (!optHMAC1)
			{
				EQ_THROW(u8"HMACの読み出しに失敗しました。");
			}

			// HMAC算出用ストリームを作成する
			stream->Seek(pos, SeekOrigin::End);
			auto partial = PartialStream::CreateFromStream(stream, beginPos, stream->Position() - beginPos);
			if (!partial)
			{
				EQ_THROW(u8"パーシャルストリームの作成に失敗しました。");
			}

			// このファイルのHMACを算出する
			auto optHMAC2 = SHA256::HMAC(partial, secretKey);
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

