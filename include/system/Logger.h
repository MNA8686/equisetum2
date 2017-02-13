#if !defined(_EQLOGGER_H_)
#define _EQLOGGER_H_

#include <cstdint>

namespace Equisetum2
{
	enum class LogLevel : int
	{
		VERBOSE = 1,
		DEBUG,
		INFO,
		WARN,
		ERROR,
		CRITICAL,
	};

	class Logger
	{
	public:
		static void SetPriority(LogLevel level);
		static void Output(LogLevel level, const char* format, ...);
		static void OutputV(LogLevel level, const char* format, va_list arg);
		static void OutputVerbose(const char* format, ...);
		static void OutputDebug(const char* format, ...);
		static void OutputInfo(const char* format, ...);
		static void OutputWarn(const char* format, ...);
		static void OutputError(const char* format, ...);
		static void OutputCritical(const char* format, ...);
	};
}

#endif
