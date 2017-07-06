/**
* @file SHA256.hpp
* @brief SHA256によるハッシュ算出、及びHMAC算出クラス(ハッシュを手っ取り早く使いたい人のためのクラス)
* @author MNA
* @date 2017/05/18 新規作成
*/
#if !defined(_EQSHA256_H_)
#define _EQSHA256_H_

#include "type/String.h"
#include "type/Optional.h"
#include "stream/IStream.h"

namespace Equisetum2
{
	/**
	* SHA256のハッシュ値格納構造体(ハッシュを手っ取り早く使いたい人のためのクラス)
	*/
	struct SHA256Value
	{
		uint8_t value[32];	/// ハッシュ値

		/**
		* @brief SHA256ハッシュ値の文字列を得る
		* @return SHA256ハッシュ値の文字列
		*/
		String ToString() const;

		/**
		* @brief ハッシュ値が同一かどうかチェック
		* @return 同一の場合、真
		*/
		bool operator == (const SHA256Value& other) const;

		/**
		* @brief ハッシュ値が非同一かどうかチェック
		* @return 同一ではない場合、真
		*/
		bool operator != (const SHA256Value& other) const;
	};

	/**
	* SHA256によるハッシュ算出、及びHMAC算出クラス
	*/
	class SHA256
	{
	public:
		/**
		* @brief SHA256ハッシュ値を算出する
		* @param stream 入力ストリーム 入力時のシーク位置から最後までを読み出す
		* @return SHA256ハッシュ値
		*/
		static Optional<SHA256Value> Encrypt(std::shared_ptr<IStream> stream);

		/**
		* @brief SHA256 HMAC値を算出する
		* @param stream 入力ストリーム 入力時のシーク位置から最後までを読み出す
		* @param secretKey 秘密鍵
		* @return SHA256ハッシュ値
		*/
		static Optional<SHA256Value> HMAC(std::shared_ptr<IStream> stream, const String& secretKey);
	};
}

#endif
