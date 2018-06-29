#if !defined(_EQFILESTREAM_H_)
#define _EQFILESTREAM_H_

#include "type/String.h"
#include "IStream.h"
#include <memory>

namespace Equisetum2
{
	class FileStream : public IStream
	{
	public:

		class Method
		{
		public:
			static const int Read = (1 << 0);
			static const int Write = (1 << 1);
			static const int Append = (1 << 2);
			static const int Create = (1 << 3);

			// Read : "rb"
			// Write : "wb"
			// Append : "ab"
			// Append | Read : "ab+"
			// Read | Write : "rb+"
			// Read | Write | Create : "wb+"
		};

		// ファクトリー
		static std::shared_ptr<FileStream> CreateFromPath(const String& strPath, int openMethod = Method::Read);
		static std::shared_ptr<FileStream> NewFileFromPath(const String& strPath);

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

	protected:

		explicit FileStream();
		virtual ~FileStream();
		virtual bool OpenFromPath(const String& strPath, int openMethod);

	private:

		class StreamImpl;
		std::shared_ptr<StreamImpl> m_pImpl;
		int m_method = 0;
		String m_url;

		FileStream(const FileStream&) = delete;				// コピーコンストラクタ封じ
		FileStream& operator= (const FileStream&) = delete;	// コピーコンストラクタ封じ
	};
}

#endif
