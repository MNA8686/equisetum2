#include "stream/TextWriter.h"

namespace Equisetum2
{
	std::shared_ptr<TextWriter> TextWriter::CreateFromStream(std::shared_ptr<IStream> stream)
	{
		class TextWriterDummy : public TextWriter
		{
			// このファクトリー以外でインスタンスを作らせないようにコンストラクタとデストラクタを
			// protected宣言しているが、そのままだとmake_sharedでエラーになるので一旦ダミークラスを挟む

		public:
			explicit TextWriterDummy(std::shared_ptr<IStream>& stream) : TextWriter(stream) {};
		};

		std::shared_ptr<TextWriter> inst;
		if (stream &&
			stream->CanWrite())
		{
			if (auto inst_ = std::make_shared<TextWriterDummy>(stream))
			{
				inst = inst_;
			}
		}
		return inst;
	}

	TextWriter::TextWriter(std::shared_ptr<IStream>& stream)
	{
		m_stream = stream;
	}

	TextWriter::~TextWriter()
	{
	}

	bool TextWriter::Write(const String& code)
	{
		std::vector<uint8_t> out;
		bool ret = false;

		// 改行コードを処理
		if (code == "\r\n" ||
			code == "\r" ||
			code == "\n")
		{
			out.push_back('\r');
			out.push_back('\n');
		}
		else if(code.size_by_codepoints() == 1)
		{
			for (auto c : code)
			{
				out.push_back((uint8_t)c);
			}
		}

		if (out.size() > 0)
		{
			auto optWrite = m_stream->Write(out);
			if (optWrite &&
				*optWrite == out.size())
			{
				ret = true;
			}
		}

		return ret;
	}

	bool TextWriter::WriteLine(const String& str)
	{
		std::vector<uint8_t> out;
		bool ret = false;

		for (auto c : str)
		{
			out.push_back((uint8_t)c);
		}
		out.push_back('\r');
		out.push_back('\n');

		auto optWrite = m_stream->Write(out);
		if (optWrite &&
			*optWrite == out.size())
		{
			ret = true;
		}

		return ret;
	}
}
