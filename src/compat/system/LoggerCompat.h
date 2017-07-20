#if !defined(_EQLOGGERCOMPAT_H_)
#define _EQLOGGERCOMPAT_H_

#include "system/Singleton.h"
#include "system/Logger.h"
#include <memory>

namespace Equisetum2
{
	class LoggerCompat
	{
	public:
		friend Singleton<LoggerCompat>;	// シングルトンからインスタンスを作成してもらえるようにする

		void SetPriority(LogLevel level);
		void OutputV(LogLevel level, const char* format, va_list arg);

	private:

		class Impl;
		std::shared_ptr<Impl> m_Impl;
		LoggerCompat();	// インスタンス作成封じ
	};
}

#endif
