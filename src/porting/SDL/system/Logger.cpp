#include "system/Logger.h"
#include "SDL.h"

namespace Equisetum2
{
	static SDL_LogPriority ToLogPriority(LogLevel level)
	{
		SDL_LogPriority pri = SDL_LOG_PRIORITY_VERBOSE;
		switch (level)
		{
		case LogLevel::DEBUG:
			pri = SDL_LOG_PRIORITY_DEBUG;
			break;
		case LogLevel::INFO:
			pri = SDL_LOG_PRIORITY_INFO;
			break;
		case LogLevel::WARN:
			pri = SDL_LOG_PRIORITY_WARN;
			break;
		case LogLevel::ERROR:
			pri = SDL_LOG_PRIORITY_ERROR;
			break;
		case LogLevel::CRITICAL:
			pri = SDL_LOG_PRIORITY_CRITICAL;
			break;
		}

		return pri;
	}

	void Logger::SetPriority(LogLevel level)
	{
		SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, ToLogPriority(level));
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
		SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, ToLogPriority(level), format, arg);
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
