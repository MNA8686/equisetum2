/**
* @file IStream.h
* @brief ストリームの抽象クラス
* @author MNA
* @date 2017/02/13 新規作成
*/
#if !defined(_EQSTREAM_H_)
#define _EQSTREAM_H_

#include <ctype.h>
#include <cstdint>
#include <vector>
#include <memory>
#include "type/Optional.h"

namespace Equisetum2
{
	/**
	* @enum SeekOrigin
	* シーク方法の指定
	*/
	enum class SeekOrigin : int
	{
		Begin = 1,	/// ファイルの先頭から
		Current,	/// 現在の位置から
		End			/// ファイルの終端から
	};

	/**
	* ストリームの抽象クラス。<br>
	* ストリームを扱うクラスはこれを派生して使う。
	*/
	class IStream
	{
	public:
		/**
		* @brief read可能かどうか
		* @return read可否
		*/
		virtual bool CanRead() const = 0;

		/**
		* @brief seek可能かどうか
		* @return seek可否
		*/
		virtual bool CanSeek() const = 0;

		/**
		* @brief write可能かどうか
		* @return write可否
		*/
		virtual bool CanWrite() const = 0;

		/**
		* @brief ストリーム内での現在位置を返す
		* @return ストリーム内での現在位置
		*/
		virtual int64_t Position() const = 0;

		/**
		* @brief ストリームのサイズを返す
		* @return ストリームのサイズ
		*/
		virtual int64_t Length() const = 0;

		/**
		* @brief 現在位置から終端までの内容を別のストリームに出力する
		* @param stream 出力先ストリーム
		* @return 成否
		*
		* @detail 書き込み先ストリームが書き込み可能でなければならない
		*/
		virtual bool CopyTo(std::shared_ptr<IStream> stream) = 0;

		/**
		* @brief Seekを行う
		* @param offset オフセット
		* @param origin 基準とする位置
		* @return seek後の位置
		*/
		virtual int64_t Seek(int64_t offset, SeekOrigin origin) = 0;

		/**
		* @brief ストリームに書き込む
		* @param data 読み出しデータ格納バッファ
		* @param size 読み出しバイト数
		* @return null:読み出し失敗、>=0:読み出したバイト数
		*/
		virtual const Optional<size_t> Read(uint8_t *data, size_t size) = 0;

		/**
		* @brief ストリームに書き込む
		* @param data 書き込みデータ格納バッファ
		* @param size 書き込みバイト数
		* @return null:書き込み失敗、>=0:書き込んだバイト数
		*/
		virtual const Optional<size_t> Write(const uint8_t *data, size_t size) = 0;

		/**
		* @brief 1バイト読み出す
		* @return >=0:読み出した内容(下位1バイト)、<0:読み出し失敗
		*/
		virtual int ReadByte() = 0;

		/**
		* @brief 1バイト書き込む
		* @param writeData 書き込みたいデータ
		* @return 1:成功、1以外:失敗
		*/
		virtual int WriteByte(uint8_t writeData) = 0;
	};
}

#endif
