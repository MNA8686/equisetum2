#include "stream/TextWriter.h"
#include "system/Exception.hpp"
#include "system/Logger.h"

namespace Equisetum2
{
	std::shared_ptr<TextWriter> TextWriter::CreateFromStream(std::shared_ptr<IStream> stream)
	{
		std::shared_ptr<TextWriter> inst;

		EQ_DURING
		{
			if (!stream)
			{
				EQ_THROW(u8"有効なストリームではありません。");
			}

			if (!stream->CanWrite())
			{
				EQ_THROW(u8"ライト属性が必要です。");
			}

			class TextWriterDummy : public TextWriter
			{
			public:
				explicit TextWriterDummy(std::shared_ptr<IStream>& stream) : TextWriter(stream) {};
			};

			auto inst_ = std::make_shared<TextWriterDummy>(stream);

			if (!inst_)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			inst = inst_;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER
			
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
		else
		{
			Logger::OutputError(u8"入力可能なのは1コードポイントだけです。");
		}

		if (out.size() > 0)
		{
			auto optWrite = m_stream->Write(&out[0], out.size());
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

		auto optWrite = m_stream->Write(&out[0], out.size());
		if (optWrite &&
			*optWrite == out.size())
		{
			ret = true;
		}

		return ret;
	}
}
