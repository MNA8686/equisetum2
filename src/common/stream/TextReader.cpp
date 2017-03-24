#include "stream/TextReader.h"
#include "system/Exception.hpp"
#include "system/Logger.h"

namespace Equisetum2
{
	std::shared_ptr<TextReader> TextReader::CreateFromStream(std::shared_ptr<IStream> stream)
	{
		std::shared_ptr<TextReader> inst;

		EQ_DURING
		{
			if (!stream)
			{
				EQ_THROW(u8"有効なストリームではありません。");
			}

			if (!stream->CanRead())
			{
				EQ_THROW(u8"リード属性が必要です。");
			}

			if (!stream->CanSeek())
			{
				EQ_THROW(u8"シーク属性が必要です。");
			}

			class TextReaderDummy : public TextReader
			{
				// このファクトリー以外でインスタンスを作らせないようにコンストラクタとデストラクタを
				// protected宣言しているが、そのままだとmake_sharedでエラーになるので一旦ダミークラスを挟む

			public:
				explicit TextReaderDummy(std::shared_ptr<IStream>& stream) : TextReader(stream) {};
			};

			auto inst_ = std::make_shared<TextReaderDummy>(stream);

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

			// 2バイト目以降に来るはずの文字が来てたら処理終了
			if (readByte >= 0x80 && readByte <= 0xBF)
			{
				Logger::OutputError(u8"不正なUTF-8です。");
				break;
			}

			// 1バイト目は確定
			code += (uint8_t)readByte;

			// ASCII?
			if (readByte <= 0x7F)
			{
				// 改行コードを処理
				if (code == "\r" ||
					code == "\n")
				{
					if (code == "\r")
					{
						auto readByte = m_stream->ReadByte();
						if (readByte >= 0 &&
							readByte != '\n')
						{
							// 読み戻す
							m_stream->Seek(-1, SeekOrigin::Current);
						}
					}

					code = "\r\n";
				}

				result = true;
				break;
			}

			// 何バイトで構成されているかを算出
			auto byte = (uint8_t)readByte;
			int count = 0;
			while (byte & 0x80)
			{
				byte <<= 1;
				count++;
			}

			// 必要なバイト数を取り出す
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
				Logger::OutputError(u8"不正なUTF-8です。");
				break;
			}

			// BOMチェック
			if (code.size() == 3)
			{
				const char bom[] = { '\xEF', '\xBB', '\xBF', '\0' };

				if (code == bom)
				{
					// どうやらBOMだったようなので無視して次の文字を取得する
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
