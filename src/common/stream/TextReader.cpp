#include "stream/TextReader.h"

namespace Equisetum2
{
	std::shared_ptr<TextReader> TextReader::CreateFromStream(std::shared_ptr<IStream> stream)
	{
		class TextReaderDummy : public TextReader
		{
			// ���̃t�@�N�g���[�ȊO�ŃC���X�^���X����点�Ȃ��悤�ɃR���X�g���N�^�ƃf�X�g���N�^��
			// protected�錾���Ă��邪�A���̂܂܂���make_shared�ŃG���[�ɂȂ�̂ň�U�_�~�[�N���X������

		public:
			explicit TextReaderDummy(std::shared_ptr<IStream>& stream) : TextReader(stream) {};
		};

		std::shared_ptr<TextReader> inst;
		if (stream &&
			stream->CanRead() &&
			stream->CanSeek())
		{
			if (auto inst_ = std::make_shared<TextReaderDummy>(stream))
			{
				inst = inst_;
			}
		}
		return inst;
	}

	TextReader::TextReader(std::shared_ptr<IStream>& stream)
	{
		m_stream = stream;
	}

	TextReader::~TextReader()
	{
	}

	String TextReader::Read()
	{
		String code;
		bool result = false;

		do
		{
			auto readByte = m_stream->ReadByte();
			if (readByte < 0)
			{
				break;
			}

			// 2�o�C�g�ڈȍ~�ɗ���͂��̕��������Ă��珈���I��
			if (readByte >= 0x80 && readByte <= 0xBF)
			{
				break;
			}

			// 1�o�C�g�ڂ͊m��
			code += (uint8_t)readByte;

			// ASCII?
			if (readByte <= 0x7F)
			{
				// ���s�R�[�h������
				if (code == "\r" ||
					code == "\n")
				{
					if (code == "\r")
					{
						auto readByte = m_stream->ReadByte();
						if (readByte >= 0 &&
							readByte != '\n')
						{
							// �ǂݖ߂�
							m_stream->Seek(-1, SeekOrigin::Current);
						}
					}

					code = "\r\n";
				}

				result = true;
				break;
			}

			// ���o�C�g�ō\������Ă��邩���Z�o
			auto byte = (uint8_t)readByte;
			int count = 0;
			while (byte & 0x80)
			{
				byte <<= 1;
				count++;
			}

			// �K�v�ȃo�C�g�������o��
			for (int i = 0; i < count-1; i++)
			{
				readByte = m_stream->ReadByte();
				if (readByte >= 0x80 && readByte <= 0xBF)
				{
					code += (uint8_t)readByte;
				}
			}

			if (code.size() != count)
			{
				break;
			}

			// BOM�`�F�b�N
			if (code.size() == 3)
			{
				const char bom[] = { '\xEF', '\xBB', '\xBF', '\0' };

				if (code == bom)
				{
					// �ǂ����BOM�������悤�Ȃ̂Ŗ������Ď��̕������擾����
					code.clear();
					continue;
				}
			}

			result = true;
		} while (!result);

		return result ? code : "";
	}

	Optional<String> TextReader::ReadLine()
	{
		String text;
		bool newline = false;

		String code;
		while ((code = Read()).size() > 0)
		{
			if (code == "\r\n")
			{
				newline = true;
				break;
			}

			text += code;
		}

		Optional<String> optText;
		if (newline || text.size() > 0)
		{
			optText = text;
		}

		return optText;
	}
}
