#if !defined(_EQTEXTREADER_H_)
#define _EQTEXTREADER_H_

#include "type/String.h"
#include "IStream.h"
#include <memory>

namespace Equisetum2
{
	class TextReader
	{
	public:

		// ファクトリー
		static std::shared_ptr<TextReader> CreateFromStream(std::shared_ptr<IStream> stream);

		// 1コードポイント取得
		// 改行は \r\n として取得できる
		virtual String Read();

		// 1行取得する
		virtual Optional<String> ReadLine();

	protected:

		explicit TextReader(std::shared_ptr<IStream>& stream);
		virtual ~TextReader();

	private:

		TextReader(const TextReader&) = delete;				// コピーコンストラクタ封じ
		TextReader& operator= (const TextReader&) = delete;	// コピーコンストラクタ封じ

		std::shared_ptr<IStream> m_stream;
	};
}

#endif
