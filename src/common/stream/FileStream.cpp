#include "stream/FileStream.h"

#include "stream/FileStream.inl"

namespace Equisetum2
{
	// �ǂ��ɒu����
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

	std::shared_ptr<FileStream> FileStream::CreateFromPath(const String& strPath, int openMethod)
	{
		class EqFileStreamDummy : public FileStream
		{
			// ���̃t�@�N�g���[�ȊO�ŃC���X�^���X����点�Ȃ��悤�ɃR���X�g���N�^�ƃf�X�g���N�^��
			// protected�錾���Ă��邪�A���̂܂܂���make_shared�ŃG���[�ɂȂ�̂ň�U�_�~�[�N���X������
		};

		std::shared_ptr<FileStream> inst;
		if (auto inst_ = std::make_shared<EqFileStreamDummy>())
		{
			if (inst_->OpenFromPath(strPath, openMethod))
			{
				inst = inst_;
			}
		}
		return inst;
	}

	FileStream::FileStream()
	{
	}

	FileStream::~FileStream()
	{
	}

	bool FileStream::OpenFromPath(const String& strPath, int openMethod)
	{
		m_method = openMethod;	// �I�[�v���̐ݒ��ێ�
		m_pImpl = StreamImpl::OpenFromPath(strPath, openMethod);
		return m_pImpl != nullptr;
	}

	bool FileStream::CanRead() const
	{
		return !!(m_method & Method::Read);
	}

	bool FileStream::CanSeek() const
	{
		// �Œ�
		return true;
	}

	bool FileStream::CanWrite() const
	{
		// Write�ŃI�[�v���ł������Ă��Ƃ͏������݂ł���̂ł��낤
		return !!(m_method & Method::Write);
	}

	int64_t FileStream::Position() const
	{
		return m_pImpl->Position();
	}

	int64_t FileStream::Length() const
	{
		return m_pImpl->Length();
	}

	bool FileStream::CopyTo(std::shared_ptr<IStream> stream)
	{
		return Equisetum2::CopyTo(this, stream ? stream.get() : nullptr);
	}

	int64_t FileStream::Seek(int64_t offset, SeekOrigin origin)
	{
		return m_pImpl->Seek(offset, origin);
	}

	const Optional<size_t> FileStream::Read(std::vector<uint8_t>& vByteArray, size_t begin, size_t size)
	{
		Optional<size_t> optSize;

		if (begin + size <= vByteArray.size())
		{
			optSize = m_pImpl->Read(&vByteArray[begin], size);
		}

		return optSize;
	}

	const Optional<size_t> FileStream::Write(const std::vector<uint8_t>& vByteArray, size_t begin, size_t size)
	{
		Optional<size_t> optSize;

		if (begin + size <= vByteArray.size())
		{
			optSize = m_pImpl->Write(&vByteArray[begin], size);
		}

		return optSize;
	}

	int FileStream::ReadByte()
	{
		return m_pImpl->ReadByte();
	}

	int FileStream::WriteByte(uint8_t writeData)
	{
		return m_pImpl->WriteByte(writeData);
	}
}
