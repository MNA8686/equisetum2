#if !defined(_EQVARIABLEMEMORYSTREAM_H_)
#define _EQVARIABLEMEMORYSTREAM_H_

#include "type/String.h"
#include "IStream.h"
#include <memory>
#include <vector>

namespace Equisetum2
{
	class VariableMemoryStream : public IStream
	{
	public:

		// ファクトリー
		static std::shared_ptr<VariableMemoryStream> Create(int64_t size=0);

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
		virtual String Url() override;

		virtual const void* Data() const;

		virtual ~VariableMemoryStream() = default;

	protected:

		explicit VariableMemoryStream() = default;

	private:


		String m_url;
		std::vector<uint8_t> m_buf;
		int64_t m_pos = 0;

		VariableMemoryStream(const VariableMemoryStream&) = delete;				// コピーコンストラクタ封じ
		VariableMemoryStream& operator= (const VariableMemoryStream&) = delete;	// コピーコンストラクタ封じ
	};
}

#endif
