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

		// �t�@�N�g���[
		static std::shared_ptr<TextReader> CreateFromStream(std::shared_ptr<IStream> stream);

		// 1�R�[�h�|�C���g�擾
		// ���s�� \r\n �Ƃ��Ď擾�ł���
		virtual String Read();

		// 1�s�擾����
		virtual Optional<String> ReadLine();

	protected:

		explicit TextReader(std::shared_ptr<IStream>& stream);
		virtual ~TextReader();

	private:

		TextReader(const TextReader&) {}				// �R�s�[�R���X�g���N�^����
		TextReader& operator= (const TextReader&) {}	// �R�s�[�R���X�g���N�^����

		std::shared_ptr<IStream> m_stream;
	};
}

#endif
