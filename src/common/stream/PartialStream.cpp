/**
* @file PartialStream.cpp
* @brief あるストリームの一部分をあたかも単独のストリームであるかのように使えるクラス
* @author MNA
* @date 2017/08/09 新規作成
*/
#include "stream/PartialStream.hpp"
#include "system/Exception.hpp"
#include "system/Logger.h"

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

	PartialStream::PartialStream(std::shared_ptr<IStream>& stream)
	{
		m_stream = stream;
	}

	std::shared_ptr<PartialStream> PartialStream::CreateFromStream(std::shared_ptr<IStream> stream, int64_t begin, int64_t length)
	{
		std::shared_ptr<PartialStream> inst;

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

			if (begin < 0 ||
				length < 0)
			{
				EQ_THROW(u8"引数の値が不正です。");
			}

			if (begin + length > stream->Length())
			{
				EQ_THROW(u8"引数の値が不正です。");
			}

			class PartialStreamDummy : public PartialStream
			{
			public:
				explicit PartialStreamDummy(std::shared_ptr<IStream>& stream) : PartialStream(stream) {};
			};

			auto inst_ = std::make_shared<PartialStreamDummy>(stream);

			if (!inst_)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			stream->Seek(begin, SeekOrigin::Begin);

			inst_->m_stream = stream;
			inst_->m_beginPos = begin;
			inst_->m_length = length;

			inst = inst_;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return inst;
	}

	bool PartialStream::CanRead() const
	{
		return m_stream->CanRead();
	}

	bool PartialStream::CanSeek() const
	{
		return m_stream->CanSeek();
	}

	bool PartialStream::CanWrite() const
	{
		return m_stream->CanWrite();
	}

	int64_t PartialStream::Position() const
	{
		return m_stream->Position() - m_beginPos;
	}

	int64_t PartialStream::Length() const
	{
		return m_length;
	}

	bool PartialStream::CopyTo(std::shared_ptr<IStream> stream)
	{
		return Equisetum2::CopyTo(this, stream ? stream.get() : nullptr);
	}

	int64_t PartialStream::Seek(int64_t offset, SeekOrigin origin)
	{
		if (origin == SeekOrigin::Begin)
		{
			auto offset_ = offset;
			if (offset_ < 0)
			{
				offset_ = 0;
			}
			if (offset > Length())
			{
				offset_ = Length();
			}

			m_stream->Seek(m_beginPos + offset_, SeekOrigin::Begin);
		}
		else if (origin == SeekOrigin::Current)
		{
			auto offset_ = offset;
			if (Position() + offset_ > Length())
			{
				offset_ = Length() - Position();
			}
			else if (Position() + offset < 0)
			{
				offset_ = -Position();
			}

			m_stream->Seek(m_beginPos + Position() + offset_, SeekOrigin::Begin);
		}
		else if (origin == SeekOrigin::End)
		{
			auto offset_ = offset;
			if (offset_ > 0)
			{
				offset_ = 0;
			}
			if (offset < -Length())
			{
				offset_ = -Length();
			}

			m_stream->Seek(m_beginPos + Length() + offset_, SeekOrigin::Begin);
		}

		return Position();
	}

	const Optional<size_t> PartialStream::Read(uint8_t *data, size_t size)
	{
		auto readSize = Position() + size <= Length() ? size : Length() - Position();

		return m_stream->Read(data, (size_t)readSize);
	}

	const Optional<size_t> PartialStream::Write(const uint8_t *data, size_t size)
	{
		return{};
	}

	int PartialStream::ReadByte()
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

	int PartialStream::WriteByte(uint8_t writeData)
	{
		return -1;
	}
}
