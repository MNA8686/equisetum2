#if !defined(_EQEXCEPTION_H_)
#define _EQEXCEPTION_H_

#include "type/String.h"

namespace Equisetum2
{
	class Exception : public std::exception
	{
	public:

		Exception() {};

		Exception(const String& msg, const String& file, int line, const String& function) :
			m_msg(msg), m_file(file), m_line(line), m_function(function)
		{
			m_what.format("%s (%d): In function '%s()': %s", m_file.c_str(), m_line, m_function.c_str(), m_msg.c_str());
		}

		const char* what() const noexcept override
		{
			return m_what.c_str();
		}

		operator bool() const
		{
			// �����ݒ肳��Ă���ΗL���Ƃ݂Ȃ�
			return m_what.size() > 0;
		}

	private:

		String m_what;
		String m_msg;	// �G���[���b�Z�[�W
		String m_file;	// �t�@�C����
		int m_line = 0;	// �s�ԍ�
		String m_function;	// �֐���
	};
}

#define EQ_DURING { Exception _e_; {
#define EQ_HANDLER } __ERROR__: if(_e_){
#define EQ_END_HANDLER }}
#define EQ_THROW(msg) do{ _e_ = Exception((msg), __FILE__, __LINE__, __FUNCTION__); goto __ERROR__;}while(0)
#define EQ_GET_HANDLER() (_e_)

#endif
