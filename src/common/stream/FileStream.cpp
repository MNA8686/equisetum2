#include "stream/FileStream.h"
#include "stream/SDLBinderRWops.h"

namespace Equisetum2
{
	class FileStream::StreamImpl : public SDLBinderRWops
	{
	public:
		StreamImpl(SDL_RWops* pRWops) : SDLBinderRWops(pRWops){}
	};

	std::shared_ptr<FileStream> FileStream::CreateFromPath(const String& strPath, const String& strMethod)
	{
		class EqFileStreamDummy : public FileStream
		{
			// このファクトリー以外でインスタンスを作らせないようにコンストラクタとデストラクタを
			// protected宣言しているが、そのままだとmake_sharedでエラーになるので一旦ダミークラスを挟む
		};

		std::shared_ptr<FileStream> inst;
		auto inst_ = std::make_shared<EqFileStreamDummy>();
		if (inst_)
		{
			if (inst_->OpenFromPath(strPath, strMethod))
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

	bool FileStream::OpenFromPath(const String& strPath, const String& strMethod)
	{
		auto pRWops = SDL_RWFromFile(strPath.c_str(), strMethod.c_str());
		if(pRWops)
		{
			m_pImpl = std::make_shared<StreamImpl>(pRWops);
		}

		return m_pImpl != nullptr;
	}

#if 0
	bool FileStream::CanRead() const
	{
		return true;
	}

	bool FileStream::CanSeek() const
	{
		return true;
	}

	bool FileStream::CanWrite() const
	{
		return true;	// とは限らない
	}
#endif

	int64_t FileStream::Position() const
	{
		return m_pImpl->Position();
	}

	int64_t FileStream::Length() const
	{
		return m_pImpl->Length();
	}

#if 0
	void FileStream::CopyTo(IStream& stream)
	{

	}
#endif

	int64_t FileStream::Seek(int64_t offset, SeekOrigin origin)
	{
		return m_pImpl->Seek(offset, origin);
	}

	size_t FileStream::Read(std::vector<uint8_t>& vByteArray, size_t begin, size_t size)
	{
		return m_pImpl->Read(&vByteArray[begin], size);
	}

	size_t FileStream::Write(const std::vector<uint8_t>& vByteArray, size_t begin, size_t size)
	{
		return m_pImpl->Write(&vByteArray[begin], size);
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
