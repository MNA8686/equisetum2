#include "system/Logger.h"
#include "system/Singleton.h"
#include "system/LoggerCompat.h"

#include <stdarg.h>

namespace Equisetum2
{
	void Logger::SetPriority(LogLevel level)
	{
		Singleton<LoggerCompat>::GetInstance()->SetPriority(level);
	}

	void Logger::Output(LogLevel level, const char* format, ...)
	{
		va_list arg;

		va_start(arg, format);
		OutputV(level, format, arg);
		va_end(arg);
	}

	void Logger::OutputV(LogLevel level, const char* format, va_list arg)
	{
		Singleton<LoggerCompat>::GetInstance()->OutputV(level, format, arg);
	}

	void Logger::OutputVerbose(const char* format, ...)
	{
		va_list arg;

		va_start(arg, format);
		OutputV(LogLevel::VERBOSE, format, arg);
		va_end(arg);
	}

	void Logger::OutputDebug(const char* format, ...)
	{
		va_list arg;

		va_start(arg, format);
		OutputV(LogLevel::DEBUG, format, arg);
		va_end(arg);
	}

	void Logger::OutputInfo(const char* format, ...)
	{
		va_list arg;

		va_start(arg, format);
		OutputV(LogLevel::INFO, format, arg);
		va_end(arg);
	}

	void Logger::OutputWarn(const char* format, ...)
	{
		va_list arg;

		va_start(arg, format);
		OutputV(LogLevel::WARN, format, arg);
		va_end(arg);
	}

	void Logger::OutputError(const char* format, ...)
	{
		va_list arg;

		va_start(arg, format);
		OutputV(LogLevel::ERROR, format, arg);
		va_end(arg);
	}

	void Logger::OutputCritical(const char* format, ...)
	{
		va_list arg;

		va_start(arg, format);
		OutputV(LogLevel::CRITICAL, format, arg);
		va_end(arg);
	}
}
