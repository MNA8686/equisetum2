/**
* @file CryptStream.cpp
* @brief 与えたキーを元に暗号と複合を行うクラス
* @author MNA
* @date 2017/08/10 新規作成
*/
#include "stream/CryptStream.hpp"
#include "system/Exception.hpp"
#include "system/Logger.h"
#include "stream/MemoryStream.h"

namespace Equisetum2
{
	// どこに置こう
	static bool CopyTo(IStream* pSrc, IStream* pDst)
	{
		auto result = false;

		EQ_DURING
		{
			if (!pSrc)
			{
				EQ_THROW(u8"読み出し元が有効なストリームではありません。");
			}

			if (!pSrc->CanRead())
			{
				EQ_THROW(u8"読み出し元にリード属性が必要です。");
			}

			if (!pDst)
			{
				EQ_THROW(u8"書き込み先が有効なストリームではありません。");
			}

			if (!pDst->CanWrite())
			{
				EQ_THROW(u8"書き込み先にライト属性が必要です。");
			}

			std::vector<uint8_t> buf(1 * 1024 * 1024);

			while (auto readSize = pSrc->Read(&buf[0], buf.size()))
			{
				if (*readSize == 0)
				{
					result = true;
					break;
				}

				auto writeSize = pDst->Write(&buf[0], *readSize);
				if (!writeSize || *writeSize < *readSize)
				{
					EQ_THROW(u8"書き込みに失敗しました。");
					break;
				}
			}
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return result;
	}

	std::shared_ptr<CryptStream> CryptStream::CreateFromStream(std::shared_ptr<IStream> stream, const String& key)
	{
		std::shared_ptr<CryptStream> inst;

		EQ_DURING
		{
			if (!stream)
			{
				EQ_THROW(u8"有効なストリームではありません。");
			}

			if (key.empty())
			{
				EQ_THROW(u8"キーが設定されていません。");
			}

			class CryptStreamDummy : public CryptStream
			{
			public:
				explicit CryptStreamDummy(std::shared_ptr<IStream>& stream) : CryptStream(stream) {};
			};

			auto inst_ = std::make_shared<CryptStreamDummy>(stream);
			if (!inst_)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			auto mem = MemoryStream::CreateFromBuf(key.c_str(), key.size());
			if (!mem)
			{
				EQ_THROW(u8"メモリストリームの作成に失敗しました。");
			}

			// キーからSHA256ハッシュを作成し、それを元にcryptを行う
			auto sha256 = SHA256::Encrypt(mem);
			if (!sha256)
			{
				EQ_THROW(u8"キーの作成に失敗しました。");
			}

			// キーを保存
			inst_->key = *sha256;

			inst_->m_url = String::Sprintf(u8"crypt:// in %s", stream->Url().c_str());

			inst = inst_;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return inst;
	}

	bool CryptStream::CanRead() const
	{
		return m_stream->CanRead();
	}

	bool CryptStream::CanSeek() const
	{
		return m_stream->CanSeek();
	}

	bool CryptStream::CanWrite() const
	{
		return m_stream->CanWrite();
	}

	int64_t CryptStream::Position() const
	{
		return m_stream->Position();
	}

	int64_t CryptStream::Length() const
	{
		return m_stream->Length();
	}

	bool CryptStream::CopyTo(std::shared_ptr<IStream> stream)
	{
		return Equisetum2::CopyTo(this, stream ? stream.get() : nullptr);
	}

	int64_t CryptStream::Seek(int64_t offset, SeekOrigin origin)
	{
		return m_stream->Seek(offset, origin);
	}

	const Optional<size_t> CryptStream::Read(uint8_t *data, size_t size)
	{
		auto pos = Position();

		auto optRead = m_stream->Read(data, size);
		if (optRead)
		{
			for (size_t i = 0; i < *optRead; i++)
			{
				data[i] ^= key.value[pos & 0x1F];

				pos++;
			}
		}

		return optRead;
	}

	const Optional<size_t> CryptStream::Write(const uint8_t *data, size_t size)
	{
		auto pos = Position();

		std::vector<uint8_t> buf(size);
		memcpy(&buf[0], data, size);

		for (size_t i = 0; i < size; i++)
		{
			buf[i] ^= key.value[pos & 0x1F];

			pos++;
		}

		return m_stream->Write(&buf[0], size);
	}

	int CryptStream::ReadByte()
	{
		int byte = -1;
		uint8_t readByte;

		auto optRead = Read(&readByte, 1);
		if (optRead &&
			*optRead == 1)
		{
			byte = readByte;
		}

		return byte;
	}

	int CryptStream::WriteByte(uint8_t writeData)
	{
		int ret = 0;

		if (auto optWrite = Write(&writeData, 1))
		{
			ret = *optWrite;
		}

		return ret;
	}

	CryptStream::CryptStream(std::shared_ptr<IStream>& stream)
	{
		m_stream = stream;
	}

	String CryptStream::Url()
	{
		return m_url;
	}
}
