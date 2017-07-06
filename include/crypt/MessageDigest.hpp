/**
* @file MessageDigest.hpp
* @brief MessageDigestによるハッシュ算出クラス
* @author MNA
* @date 2017/05/26 新規作成
*/
#if !defined(_EQMESSAGEDIGEST_H_)
#define _EQMESSAGEDIGEST_H_

#include "type/String.h"
#include <memory>

namespace Equisetum2
{
	/**
	* ハッシュ算出抽象クラス
	*/
	class IMessageDigest
	{
	public:
		/**
		* @brief インスタンスを初期化する(再利用可にする)
		* @return 成功なら真
		*/
		virtual bool Init() = 0;

		/**
		* @brief データを追加する
		* @param pBuf 入力バッファ
		* @param size 入力バッファのサイズ
		* @return 成功なら真
		*/
		virtual bool Update(const uint8_t* pBuf, size_t size) = 0;

		/**
		* @brief ハッシュ算出
		* @return ハッシュ値のバイト配列
		*/
		virtual std::vector<uint8_t> Digest() = 0;
	};

	/**
	* ハッシュ算出クラス
	*/
	class MessageDigest
	{
	public:
		/**
		* @brief ハッシュ算出クラスインスタンスを作成する
		* @param algorithm アルゴリズム名
		* @return ハッシュ算出クラスインスタンス
		*/
		static std::shared_ptr<IMessageDigest> Create(const String& algorithm);

		/**
		* @brief サポートしているアルゴリズムを取得する
		* @return アルゴリズムの文字列配列
		*/
//		static std::vector<String> GetSupportedAlgorithm();
	};

}

#endif
