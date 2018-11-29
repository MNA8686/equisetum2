#include "stream/FileStream.h"
#include "system/Exception.hpp"
#include "system/Logger.h"

#include "stream/FileStream.inl"

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

			std::vector<uint8_t> buf(4096);

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

	std::shared_ptr<FileStream> FileStream::CreateFromPath(const String& strPath, int openMethod)
	{
		std::shared_ptr<FileStream> inst;

		EQ_DURING
		{
			if (strPath.size() == 0)
			{
				EQ_THROW(u8"ファイル名が設定されていません。");
			}

			class EqFileStreamDummy : public FileStream	{};
			auto inst_ = std::make_shared<EqFileStreamDummy>();

			if (!inst_)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			if (!inst_->OpenFromPath(strPath, openMethod))
			{
				EQ_THROW(String::Sprintf(u8"ファイルのオープンに失敗しました。 name=%s, method=%d", strPath.c_str(), openMethod));
			}

			// URLを設定
			inst_->m_url = u8"file://" + strPath;

			inst = inst_;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return inst;
	}

	std::shared_ptr<FileStream> FileStream::NewFileFromPath(const String& strPath, bool read)
	{
		return CreateFromPath(strPath, Method::Create | Method::Write | (read ? Method::Read : 0));
	}

	FileStream::FileStream()
	{
	}

	FileStream::~FileStream()
	{
	}

	bool FileStream::OpenFromPath(const String& strPath, int openMethod)
	{
		m_method = openMethod;	// オープンの設定を保持
		m_pImpl = StreamImpl::OpenFromPath(strPath, openMethod);
		return m_pImpl != nullptr;
	}

	bool FileStream::CanRead() const
	{
		return !!(m_method & Method::Read);
	}

	bool FileStream::CanSeek() const
	{
		// 固定
		return true;
	}

	bool FileStream::CanWrite() const
	{
		// Writeでオープンできたってことは書き込みできるのであろう
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

	const Optional<size_t> FileStream::Read(uint8_t *data, size_t size)
	{
		Optional<size_t> optSize;

		optSize = m_pImpl->Read(data, size);

		return optSize;
	}

	const Optional<size_t> FileStream::Write(const uint8_t *data, size_t size)
	{
		Optional<size_t> optSize;

		optSize = m_pImpl->Write(data, size);

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

	String FileStream::Url()
	{
		return m_url;
	}
}
