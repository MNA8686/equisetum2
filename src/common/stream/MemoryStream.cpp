#include "stream/MemoryStream.h"
#include "system/Exception.hpp"
#include "system/Logger.h"

#include "stream/MemoryStream.inl"

namespace Equisetum2
{
	// どこに置こう
	static bool CopyTo(IStream* pSrc, IStream* pDst)
	{
		auto result = false;

		if (pSrc &&
			pDst &&
			pSrc->CanRead() &&
			pDst->CanWrite())
		{
			std::vector<uint8_t> buf(40960);

			while (auto readSize = pSrc->Read(buf, 0, buf.size()))
			{
				if (*readSize == 0)
				{
					result = true;
					break;
				}

				auto writeSize = pDst->Write(buf, 0, *readSize);
				if (!writeSize || *writeSize < *readSize)
				{
					break;
				}
			}
		}

		return result;
	}

	std::shared_ptr<MemoryStream> MemoryStream::Create(std::shared_ptr<StreamImpl>& impl)
	{
		std::shared_ptr<MemoryStream> inst;

		EQ_DURING
		{
			if (!impl)
			{
				EQ_THROW(u8"バッファの作成に失敗しました。");
			}
		
			class EqMemoryStreamDummy : public MemoryStream {};

			auto inst_ = std::make_shared<EqMemoryStreamDummy>();

			if (!inst_)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			inst_->m_pImpl = impl;
			inst = inst_;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return inst;
	}

	std::shared_ptr<MemoryStream> MemoryStream::CreateFromBuf(const void* buf, int size)
	{
		auto tmp = StreamImpl::OpenFromBuf(buf, size);
		return Create(tmp);
	}

	std::shared_ptr<MemoryStream> MemoryStream::CreateFromBuf(void* buf, int size, bool writable)
	{
		auto tmp = StreamImpl::OpenFromBuf(buf, size, writable);
		auto inst = Create(tmp);
		if (inst)
		{
			inst->m_writable = writable;
		}
		return inst;
	}

	MemoryStream::MemoryStream()
	{
	}

	MemoryStream::~MemoryStream()
	{
	}

	bool MemoryStream::CanRead() const
	{
		// 固定
		return true;
	}

	bool MemoryStream::CanSeek() const
	{
		// 固定
		return true;
	}

	bool MemoryStream::CanWrite() const
	{
		return m_writable;
	}

	int64_t MemoryStream::Position() const
	{
		return m_pImpl->Position();
	}

	int64_t MemoryStream::Length() const
	{
		return m_pImpl->Length();
	}

	bool MemoryStream::CopyTo(std::shared_ptr<IStream> stream)
	{
		return Equisetum2::CopyTo(this, stream ? stream.get() : nullptr);
	}

	int64_t MemoryStream::Seek(int64_t offset, SeekOrigin origin)
	{
		return m_pImpl->Seek(offset, origin);
	}

	const Optional<size_t> MemoryStream::Read(std::vector<uint8_t>& vByteArray, size_t begin, size_t size)
	{
		Optional<size_t> optSize;

		if (begin + size <= vByteArray.size())
		{
			optSize = m_pImpl->Read(&vByteArray[begin], size);
		}

		return optSize;
	}

	const Optional<size_t> MemoryStream::Read(std::vector<uint8_t>& vByteArray, size_t size)
	{
		return Read(vByteArray, 0, size);
	}

	const Optional<size_t> MemoryStream::Read(uint8_t *data, size_t size)
	{
		Optional<size_t> optSize;

		optSize = m_pImpl->Read(data, size);

		return optSize;
	}

	const Optional<size_t> MemoryStream::Write(const std::vector<uint8_t>& vByteArray, size_t begin, size_t size)
	{
		Optional<size_t> optSize;

		if (begin + size <= vByteArray.size())
		{
			optSize = m_pImpl->Write(&vByteArray[begin], size);
		}

		return optSize;
	}

	const Optional<size_t> MemoryStream::Write(const std::vector<uint8_t>& vByteArray, size_t size)
	{
		return Write(vByteArray, 0, size);
	}

	const Optional<size_t> MemoryStream::Write(const std::vector<uint8_t>& vByteArray)
	{
		return Write(vByteArray, 0, vByteArray.size());
	}

	const Optional<size_t> MemoryStream::Write(const uint8_t *data, size_t size)
	{
		Optional<size_t> optSize;

		optSize = m_pImpl->Write(data, size);

		return optSize;
	}

	int MemoryStream::ReadByte()
	{
		return m_pImpl->ReadByte();
	}

	int MemoryStream::WriteByte(uint8_t writeData)
	{
		return m_pImpl->WriteByte(writeData);
	}
}
