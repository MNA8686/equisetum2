/**
* @file HMAC.hpp
* @brief HMACによるハッシュ算出クラス
* @author MNA
* @date 2017/07/06 新規作成
*/
#if !defined(_EQHMAC_H_)
#define _EQHMAC_H_

#include "type/String.h"
#include "crypt/MessageDigest.hpp"
#include <memory>

namespace Equisetum2
{
	/**
	* ハッシュ算出クラス
	*/
	class HMAC
	{
	public:
		/**
		* @brief ハッシュ算出クラスインスタンスを作成する
		* @param algorithm アルゴリズム名
		* @param secretKey 秘密鍵
		* @return ハッシュ算出クラスインスタンス
		*/
		static std::shared_ptr<HMAC> Create(const String& algorithm, const String& secretKey);

		/**
		* @brief インスタンスを初期化する(再利用可にする)
		* @param algorithm アルゴリズム名
		* @param secretKey 秘密鍵
		* @return 成功なら真
		*/
		virtual bool Init(const String& algorithm, const String& secretKey);

		/**
		* @brief データを追加する
		* @param pBuf 入力バッファ
		* @param size 入力バッファのサイズ
		* @return 成功なら真
		*/
		virtual bool Update(const uint8_t* pBuf, size_t size);

		/**
		* @brief ハッシュ算出
		* @return ハッシュ値のバイト配列
		*/
		virtual std::vector<uint8_t> DoFinal();

	private:

		HMAC() {}
		~HMAC() {}
		std::shared_ptr<IMessageDigest> m_algorithm;
		uint8_t m_ipad[64];
		uint8_t m_opad[64];
	};

}

#endif
