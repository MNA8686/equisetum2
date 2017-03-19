#include "stream/MemoryStream.h"

#include "stream/MemoryStream.inl"

namespace Equisetum2
{
	// ‚Ç‚±‚É’u‚±‚¤
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
		class EqMemoryStreamDummy : public MemoryStream
		{
		};

		std::shared_ptr<MemoryStream> inst;
		if (auto inst_ = std::make_shared<EqMemoryStreamDummy>())
		{
			if (impl)
			{
				inst_->m_pImpl = impl;
				inst = inst_;
			}
		}
		return inst;
	}

	std::shared_ptr<MemoryStream> MemoryStream::CreateFromBuf(const void* buf, int size)
	{
		return Create(StreamImpl::OpenFromBuf(buf, size));
	}

	std::shared_ptr<MemoryStream> MemoryStream::CreateFromBuf(void* buf, int size, bool writable)
	{
		auto inst = Create(StreamImpl::OpenFromBuf(buf, size, writable));
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
		// ŒÅ’è
		return true;
	}

	bool MemoryStream::CanSeek() const
	{
		// ŒÅ’è
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

	int MemoryStream::ReadByte()
	{
		return m_pImpl->ReadByte();
	}

	int MemoryStream::WriteByte(uint8_t writeData)
	{
		return m_pImpl->WriteByte(writeData);
	}
}
