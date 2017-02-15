#include "system/LoggerCompat.h"

#include "system/LoggerCompat.inl"		// ���ˑ��R�[�h���C���N���[�h

namespace Equisetum2
{
	LoggerCompat::LoggerCompat()
	{
		m_Impl = std::make_shared<LoggerCompat::Impl>();
	}

	void LoggerCompat::SetPriority(LogLevel level)
	{
		m_Impl->SetPriority(level);
	}

	void LoggerCompat::OutputV(LogLevel level, const char* format, va_list arg)
	{
		m_Impl->OutputV(level, format, arg);
	}
}
