#include "type/String.h"
#include <windows.h>

namespace Equisetum2
{
	class LoggerCompat::Impl final
	{
	public:
		Impl(){}

		void SetPriority(LogLevel level)
		{
			priority = level;
		}

		void OutputV(LogLevel level, const char* format, va_list arg)
		{
			String fmt;
			fmt.formatV(format, arg);

			static const char* levelString[] =
			{
				"Unknown",
				"Verbose",
				"Debug",
				"Info",
				"Warn",
				"Error",
				"Critical",
			};
			auto out = String::Sprintf("%s: %s\r\n", levelString[static_cast<std::underlying_type<LogLevel>::type>(level)], fmt.c_str());

			OutputDebugString(reinterpret_cast<LPCWSTR>(out.to_u16().c_str()));
		}

	private:
		LogLevel priority = LogLevel::Verbose;
	};
}
