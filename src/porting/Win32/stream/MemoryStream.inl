#include "stream/MemoryStream.h"

namespace Equisetum2
{
	class MemoryStream::StreamImpl
	{
	public:
		StreamImpl() = default;

		static std::shared_ptr<StreamImpl> OpenFromBuf(const void* buf, int size)
		{
			return OpenFromBuf(const_cast<void*>(buf), size, false);
		}

		static std::shared_ptr<StreamImpl> OpenFromBuf(void* buf, int size, bool writable=true)
		{
			std::shared_ptr<StreamImpl> impl;

			if (buf && size > 0)
			{
				impl = std::make_shared<StreamImpl>();
				if (impl)
				{
					impl->m_buf = reinterpret_cast<uint8_t*>(buf);
					impl->m_bufSize = size;
				}
			}

			return impl;
		}

		inline int64_t Length() const
		{
			return m_bufSize;
		}

		inline int64_t Position() const
		{
			return m_pos;
		}

		inline size_t Read(uint8_t* pByteArray, size_t size)
		{
			if (!pByteArray)
			{
				return 0;
			}

			decltype(size) readSize = size;

			if (m_pos + static_cast<int64_t>(size) > m_bufSize)
			{
				readSize = static_cast<size_t>(m_bufSize - m_pos);
			}

			if (readSize > 0)
			{
				::memcpy(pByteArray, &m_buf[m_pos], readSize);
			}
			m_pos += readSize;

			return readSize;
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
			if (!pByteArray)
			{
				return 0;
			}

			decltype(size) writeSize = size;

			if (m_pos + static_cast<int64_t>(size) > m_bufSize)
			{
				writeSize = static_cast<size_t>(m_bufSize - m_pos);
			}

			if (writeSize > 0)
			{
				::memcpy(&m_buf[m_pos], pByteArray, writeSize);
			}
			m_pos += writeSize;

			return writeSize;
		}
		
		inline int WriteByte(uint8_t writeByte)
		{
			return (int)Write(&writeByte, 1);
		}

		inline int64_t Seek(int64_t offset, SeekOrigin origin)
		{
			if (origin == SeekOrigin::Begin)
			{
				if (offset < 0)
				{
					m_pos = 0;
				}
				else if (offset > Length())
				{
					m_pos = Length();
				}
				else
				{
					m_pos = offset;
				}
			}
			else if (origin == SeekOrigin::Current)
			{
				if (m_pos + offset > Length())
				{
					m_pos = Length();
				}
				else if (m_pos + offset < 0)
				{
					m_pos = 0;
				}
				else
				{
					m_pos += offset;
				}
			}
			else if (origin == SeekOrigin::End)
			{
				if (offset > 0)
				{
					m_pos = Length();
				}
				else if (offset < -Length())
				{
					m_pos = 0;
				}
				else
				{
					m_pos = Length() + offset;
				}
			}

			return m_pos;
		}

	private:

		uint8_t* m_buf = nullptr;
		int64_t m_bufSize = 0;
		int64_t m_pos = 0;
	};
}
