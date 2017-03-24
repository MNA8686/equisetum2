/**
* @file Logger.h
* @brief ログ出力クラス
* @author MNA
* @date 2017/02/13 新規作成
*/
#if !defined(_EQLOGGER_H_)
#define _EQLOGGER_H_

#include <cstdint>
#include <functional>

namespace Equisetum2
{
	/**
	* @enum LogLevel
	* ログの出力レベル
	*/
	enum class LogLevel : int
	{
		VERBOSE = 1,
		DEBUG,
		INFO,
		WARN,
		ERROR,
		CRITICAL,
	};

	/**
	* ログ出力クラス
	*/
	class Logger
	{
	public:
		/** コールバック型定義<br>
		* コールバック内でtrueを返すと、loggerはログを出力しない
		*/
		using cb_t = std::function<bool(const char*)>;

		/**
		* @brief メッセージ出力時に呼び出すコールバックを設定する
		* @param cb コールバックしてもらう関数オブジェクト
		*
		* @detail コールバックでtrueを返すと、Loggerでは何も出力しない。<br>
		* @detail コールバック中にLoggerを呼び出した場合、コールバックは無効として扱う。
		*/
		static void SetCallback(const cb_t& cb);

		/**
		* @brief ログレベルを設定する
		* @param level 出力されるログレベル(ここで設定したログレベル以上のもののみ出力される)
		*/
		static void SetPriority(LogLevel level);

		/**
		* @brief ログを出力する
		* @param level ログレベル
		* @param format UTF-8文字列
		*/
		static void Output(LogLevel level, const char* format, ...);

		/**
		* @brief ログを出力する
		* @param level ログレベル
		* @param format UTF-8文字列
		*/
		static void OutputV(LogLevel level, const char* format, va_list arg);

		/**
		* @brief ログをログレベルVerboseで出力する
		* @param format UTF-8文字列
		*/
		static void OutputVerbose(const char* format, ...);

		/**
		* @brief ログをログレベルDebugで出力する
		* @param format UTF-8文字列
		*/
		static void OutputDebug(const char* format, ...);

		/**
		* @brief ログをログレベルInfoで出力する
		* @param format UTF-8文字列
		*/
		static void OutputInfo(const char* format, ...);

		/**
		* @brief ログをログレベルWarnで出力する
		* @param format UTF-8文字列
		*/
		static void OutputWarn(const char* format, ...);

		/**
		* @brief ログをログレベルErrorで出力する
		* @param format UTF-8文字列
		*/
		static void OutputError(const char* format, ...);

		/**
		* @brief ログをログレベルCriticalで出力する
		* @param format UTF-8文字列
		*/
		static void OutputCritical(const char* format, ...);
	};
}

#endif
