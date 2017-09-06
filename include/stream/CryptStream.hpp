/**
* @file CryptStream.hpp
* @brief 与えたキーを元に暗号と複合を行うクラス
* @author MNA
* @date 2017/08/10 新規作成
*/
#if !defined(_EQCRYPTSTREAM_H_)
#define _EQCRYPTSTREAM_H_

#include "IStream.h"
#include "type/String.h"
#include "util/SHA256.hpp"

namespace Equisetum2
{
	/**
	* 暗号化ストリームクラス
	*/
	class CryptStream : public IStream
	{
	public:
		/**
		* @brief 与えたキーを元に暗号と複合を行うストリームを返す
		* @param stream 親ストリーム シーク属性が必要
		* @param key 暗号キー
		* @return 暗号化ストリームインスタンス 失敗した場合はnullptrを返す
		*/
		static std::shared_ptr<CryptStream> CreateFromStream(std::shared_ptr<IStream> stream, const String& key);

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
		CryptStream(std::shared_ptr<IStream>& stream);
		~CryptStream() {}

	private:
		std::shared_ptr<IStream> m_stream;	/// 親ストリーム
		SHA256Value key;		/// 暗号化キー
	};
}

#endif
