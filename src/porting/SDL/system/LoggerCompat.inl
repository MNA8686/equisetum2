#include "SDL.h"

namespace Equisetum2
{
	class LoggerCompat::Impl final
	{
	public:
		Impl(){}

		static SDL_LogPriority ToLogPriority(LogLevel level)
		{
			SDL_LogPriority pri = SDL_LOG_PRIORITY_VERBOSE;
			switch (level)
			{
			case LogLevel::Debug:
				pri = SDL_LOG_PRIORITY_DEBUG;
				break;
			case LogLevel::Info:
				pri = SDL_LOG_PRIORITY_INFO;
				break;
			case LogLevel::Warn:
				pri = SDL_LOG_PRIORITY_WARN;
				break;
			case LogLevel::Error:
				pri = SDL_LOG_PRIORITY_ERROR;
				break;
			case LogLevel::Critical:
				pri = SDL_LOG_PRIORITY_CRITICAL;
				break;
			}

			return pri;
		}

		void SetPriority(LogLevel level)
		{
			SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, ToLogPriority(level));
		}

		void OutputV(LogLevel level, const char* format, va_list arg)
		{
			SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, ToLogPriority(level), format, arg);
		}
	};
}
