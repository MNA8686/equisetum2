#if !defined(_EQFILESTREAM_H_)
#define _EQFILESTREAM_H_

#include "IStream.h"
#include <memory>

namespace Equisetum2
{
//	class StreamImpl;
	class FileStream : public IStream
	{
	public:

		// ファクトリー
		static std::shared_ptr<FileStream> CreateFromPath(const std::string& strPath, const std::string& strMethod);

//		virtual bool CanRead() const;
//		virtual bool CanSeek() const;
//		virtual bool CanWrite() const;
		virtual int64_t Position() const override;
		virtual int64_t Length() const override;
//		virtual void CopyTo(IStream& stream) override;
		virtual int64_t Seek(int64_t offset, SeekOrigin origin) override;
		virtual size_t Read(std::vector<uint8_t>& vByteArray, size_t begin, size_t size) override;
		virtual size_t Write(const std::vector<uint8_t>& vByteArray, size_t begin, size_t size) override;
		virtual int ReadByte() override;
		virtual int WriteByte(uint8_t writeData) override;

	protected:

		explicit FileStream();
		virtual ~FileStream();
		virtual bool OpenFromPath(const std::string& strPath, const std::string& strMethod);

	private:

		class StreamImpl;
		std::shared_ptr<StreamImpl> m_pImpl;

		FileStream(const FileStream&);				// コピーコンストラクタ封じ
		FileStream& operator= (const FileStream&);	// コピーコンストラクタ封じ
	};
}

#endif
