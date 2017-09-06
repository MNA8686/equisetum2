/**
* @file PartialStream.hpp
* @brief あるストリームの一部分をあたかも単独のストリームであるかのように使えるクラス
* @author MNA
* @date 2017/08/09 新規作成
*/
#if !defined(_EQPARTIALSTREAM_H_)
#define _EQPARTIALSTREAM_H_

#include "IStream.h"
#include <memory>

namespace Equisetum2
{
	/**
	* パーシャルストリームクラス
	*/
	class PartialStream : public IStream
	{
	public:
		/**
		* @brief あるストリームの一部分をあたかも単独のストリームであるかのように使用できるストリームを返す
		* @param stream 親ストリーム 読み出し属性、シーク属性が必要
		* @param begin ストリーム開始位置
		* @param length ストリームサイズ
		* @return パーシャルストリームインスタンス 失敗した場合はnullptrを返す
		*/
		static std::shared_ptr<PartialStream> CreateFromStream(std::shared_ptr<IStream> stream, int64_t begin, int64_t length);

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
		PartialStream(std::shared_ptr<IStream>& stream);
		~PartialStream() {}

	private:
		int64_t m_beginPos = 0;			/// 親ストリーム内の開始位置
		int64_t m_length = 0;			/// パーシャルストリームのサイズ
		std::shared_ptr<IStream> m_stream;	/// 親ストリーム
		String m_url;			/// URL
	};
}

#endif
