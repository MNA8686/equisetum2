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

		// �t�@�N�g���[
		static std::shared_ptr<TextWriter> CreateFromStream(std::shared_ptr<IStream> stream);

		// 1�R�[�h�|�C���g��������
		virtual bool Write(const String& code);

		// 1�s�o�͂���
		virtual bool WriteLine(const String& str);

	protected:

		explicit TextWriter(std::shared_ptr<IStream>& stream);
		virtual ~TextWriter();

	private:

		TextWriter(const TextWriter&) = delete;				// �R�s�[�R���X�g���N�^����
		TextWriter& operator= (const TextWriter&) = delete;	// �R�s�[�R���X�g���N�^����

		std::shared_ptr<IStream> m_stream;
	};
}

#endif
