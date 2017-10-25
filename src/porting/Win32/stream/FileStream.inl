#include "stream/FileStream.h"
#include <windows.h>
#include <winbase.h>

namespace Equisetum2
{
	static int64_t Seek(const std::shared_ptr<void>& handle, int64_t offset, SeekOrigin origin)
	{
		DWORD dwSeekMethod = 0;

		switch (origin)
		{
		case SeekOrigin::Begin:
			dwSeekMethod = FILE_BEGIN;
			break;
		case SeekOrigin::Current:
			dwSeekMethod = FILE_CURRENT;
			break;
		case SeekOrigin::End:
			dwSeekMethod = FILE_END;
			break;
		default:
			return -1;
		}

		LARGE_INTEGER liOffset{};
		liOffset.QuadPart = offset;

		return ::SetFilePointerEx(handle.get(), liOffset, &liOffset, dwSeekMethod) ? liOffset.QuadPart : -1;
	}

	class FileStream::StreamImpl
	{
	public:
		StreamImpl() = default;

		static std::shared_ptr<StreamImpl> OpenFromPath(const String& strPath, int openMethod)
		{
			std::shared_ptr<StreamImpl> impl;

			DWORD dwGeneric = 0;
			DWORD dwCreationDisposition = 0;

			if (openMethod & (Method::Read | Method::Write | Method::Append))
			{
				if (openMethod & Method::Append)
				{
					if ((openMethod & Method::Read))
					{
						dwGeneric |= GENERIC_READ;
					}
					dwGeneric |= GENERIC_WRITE;

					dwCreationDisposition |= OPEN_ALWAYS;
				}
				else if ((openMethod & (Method::Read | Method::Write)) == (Method::Read | Method::Write))
				{
					dwGeneric |= GENERIC_READ | GENERIC_WRITE;

					if (openMethod & Method::Create)
					{
						dwCreationDisposition |= CREATE_ALWAYS;
					}
					else
					{
						dwCreationDisposition |= OPEN_EXISTING;
					}
				}
				else if (openMethod & Method::Write)
				{
					dwGeneric |= GENERIC_WRITE;
					dwCreationDisposition |= CREATE_ALWAYS;
				}
				else
				{
					dwGeneric |= GENERIC_READ;
					dwCreationDisposition |= OPEN_EXISTING;
				}

				auto h = ::CreateFile(reinterpret_cast<LPCWSTR>(strPath.to_u16().c_str()),
					dwGeneric,
					dwGeneric & GENERIC_WRITE ? 0 : FILE_SHARE_READ, NULL,
					dwCreationDisposition,
					FILE_ATTRIBUTE_NORMAL, NULL);

				if (h != INVALID_HANDLE_VALUE)
				{
					if (auto impl_ = std::make_shared<StreamImpl>())
					{
						impl_->m_hFile = std::shared_ptr<void>(h, [](HANDLE h) {
							::CloseHandle(h);
						});

						if (impl_->m_hFile)
						{
							if (openMethod & Method::Append)
							{
								Equisetum2::Seek(impl_->m_hFile, 0, SeekOrigin::End);
							}

							impl = impl_;
						}
					}
				}
			}

			return impl;
		}

		inline int64_t Length() const
		{
			LARGE_INTEGER liSize{};

			return ::GetFileSizeEx(m_hFile.get(), &liSize) ? liSize.QuadPart : -1;
		}

		inline int64_t Position() const
		{
			return Equisetum2::Seek(m_hFile, 0, SeekOrigin::Current);
		}

		inline size_t Read(uint8_t* pByteArray, size_t size)
		{
			DWORD dwReadSize;	// 実際に読み出せたサイズ
			::ReadFile(m_hFile.get(), pByteArray, static_cast<DWORD>(size), &dwReadSize, NULL);
			return static_cast<size_t>(dwReadSize);
		}

		inline int ReadByte()
		{
			int byte = -1;

			uint8_t readByte;
			size_t readCount = Read(&readByte, 1);
			if (readCount == 1)
			{
				byte = readByte;
			}

			return byte;
		}

		inline size_t Write(const uint8_t* pByteArray, size_t size)
		{
			DWORD dwWriteSize;	// 実際に書き込めたサイズ
			::WriteFile( m_hFile.get(), pByteArray, static_cast<DWORD>(size), &dwWriteSize, NULL );
			return static_cast<size_t>(dwWriteSize);
		}

		inline int WriteByte(uint8_t writeByte)
		{
			return (int)Write(&writeByte, 1);
		}

		inline int64_t Seek(int64_t offset, SeekOrigin origin)
		{
			return Equisetum2::Seek(m_hFile, offset, origin);
		}

	private:

		// ファイルハンドル
		std::shared_ptr<void> m_hFile;
	};
}
