#include "system/Logger.h"
#include "system/Singleton.h"
#include "system/LoggerCompat.h"
#include "type/String.h"

#include <stdarg.h>

namespace Equisetum2
{
	static Logger::cb_t cb;

	void Logger::SetCallback(const Logger::cb_t& cb_)
	{
		cb = cb_;
	}

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
		// 万一コールバックからLoggerが呼び出されても永久ループにならないようにする
		static bool entry = false;
		bool out = true;

		if (!entry)
		{
			entry = true;

			if (cb && cb(level, String().formatV(format, arg).c_str()))
			{
				// コールバックがtrueを返したらLoggerはログを出力しない
				out = false;
			}

			entry = false;
		}

		if (out)
		{
			Singleton<LoggerCompat>::GetInstance()->OutputV(level, format, arg);
		}
	}

	void Logger::OutputVerbose(const char* format, ...)
	{
		va_list arg;

		va_start(arg, format);
		OutputV(LogLevel::Verbose, format, arg);
		va_end(arg);
	}

	void Logger::OutputDebug(const char* format, ...)
	{
		va_list arg;

		va_start(arg, format);
		OutputV(LogLevel::Debug, format, arg);
		va_end(arg);
	}

	void Logger::OutputInfo(const char* format, ...)
	{
		va_list arg;

		va_start(arg, format);
		OutputV(LogLevel::Info, format, arg);
		va_end(arg);
	}

	void Logger::OutputWarn(const char* format, ...)
	{
		va_list arg;

		va_start(arg, format);
		OutputV(LogLevel::Warn, format, arg);
		va_end(arg);
	}

	void Logger::OutputError(const char* format, ...)
	{
		va_list arg;

		va_start(arg, format);
		OutputV(LogLevel::Error, format, arg);
		va_end(arg);
	}

	void Logger::OutputCritical(const char* format, ...)
	{
		va_list arg;

		va_start(arg, format);
		OutputV(LogLevel::Critical, format, arg);
		va_end(arg);
	}
}
