#if !defined(_EQMEMORYSTREAM_H_)
#define _EQMEMORYSTREAM_H_

#include "type/String.h"
#include "IStream.h"
#include <memory>

namespace Equisetum2
{
	class MemoryStream : public IStream
	{
	public:

		// ファクトリー
		static std::shared_ptr<MemoryStream> CreateFromBuf(const void* buf, int size);
		static std::shared_ptr<MemoryStream> CreateFromBuf(void* buf, int size, bool writable=true);

		virtual bool CanRead() const override;
		virtual bool CanSeek() const override;
		virtual bool CanWrite() const override;
		virtual int64_t Position() const override;
		virtual int64_t Length() const override;
		virtual bool CopyTo(std::shared_ptr<IStream> stream) override;
		virtual int64_t Seek(int64_t offset, SeekOrigin origin) override;
		virtual const Optional<size_t> Read(uint8_t *data, size_t size) override;
		virtual const Optional<size_t> Write(const uint8_t *data, size_t size) override;
		virtual int ReadByte() override;
		virtual int WriteByte(uint8_t writeData) override;

	protected:

		explicit MemoryStream();
		virtual ~MemoryStream();

	private:

		class StreamImpl;
		std::shared_ptr<StreamImpl> m_pImpl;

		static std::shared_ptr<MemoryStream> Create(std::shared_ptr<StreamImpl>& impl);
		bool m_writable = false;

		MemoryStream(const MemoryStream&) = delete;				// コピーコンストラクタ封じ
		MemoryStream& operator= (const MemoryStream&) = delete;	// コピーコンストラクタ封じ
	};
}

#endif
