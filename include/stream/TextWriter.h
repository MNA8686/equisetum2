#if !defined(_EQTEXTWRITER_H_)
#define _EQTEXTWRITER_H_

#include "type/String.h"
#include "IStream.h"
#include <memory>

namespace Equisetum2
{
	class TextWriter
	{
	public:

		// ファクトリー
		static std::shared_ptr<TextWriter> CreateFromStream(std::shared_ptr<IStream> stream);

		// 1コードポイント書き込み
		virtual bool Write(const String& code);

		// 1行出力する
		virtual bool WriteLine(const String& str);

	protected:

		explicit TextWriter(std::shared_ptr<IStream>& stream);
		virtual ~TextWriter();

	private:

		TextWriter(const TextWriter&) = delete;				// コピーコンストラクタ封じ
		TextWriter& operator= (const TextWriter&) = delete;	// コピーコンストラクタ封じ

		std::shared_ptr<IStream> m_stream;
	};
}

#endif
