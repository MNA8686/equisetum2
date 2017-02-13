#if !defined(_EQSTREAM_H_)
#define _EQSTREAM_H_

#include <ctype.h>
#include <cstdint>
#include <vector>

namespace Equisetum2
{
	enum class SeekOrigin : int
	{
		Begin = 1,
		Current,
		End
	};

	class IStream
	{
	public:
//		virtual bool CanRead() const = 0;
//		virtual bool CanSeek() const = 0;
//		virtual bool CanWrite() const = 0;
		virtual int64_t Position() const = 0;
		virtual int64_t Length() const = 0;
//		virtual void CopyTo(IStream& stream) = 0;
		virtual int64_t Seek(int64_t offset, SeekOrigin origin) = 0;
		virtual size_t Read(std::vector<uint8_t>& vByteArray, size_t begin, size_t size) = 0;
		virtual size_t Write(const std::vector<uint8_t>& vByteArray, size_t begin, size_t size) = 0;
		virtual int ReadByte() = 0;
		virtual int WriteByte(uint8_t writeData) = 0;
	};
}

#endif
