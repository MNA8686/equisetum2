#include "stream/TextWriter.h"

namespace Equisetum2
{
	std::shared_ptr<TextWriter> TextWriter::CreateFromStream(std::shared_ptr<IStream> stream)
	{
		class TextWriterDummy : public TextWriter
		{
			// ���̃t�@�N�g���[�ȊO�ŃC���X�^���X����点�Ȃ��悤�ɃR���X�g���N�^�ƃf�X�g���N�^��
			// protected�錾���Ă��邪�A���̂܂܂���make_shared�ŃG���[�ɂȂ�̂ň�U�_�~�[�N���X������

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

		// ���s�R�[�h������
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
