#if !defined(_EQIMAGE_H_)
#define _EQIMAGE_H_

#include "stream/IStream.h"
#include <memory>

namespace Equisetum2
{
	union Color
	{
		struct rgba8888_t
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
			uint8_t a;
		}rgba8888;

		uint32_t pixel;
	};

	class Image
	{
	public:

		/**
		* @brief ストリームからイメージを作成する BMP、PNGのみを正式サポート
		* @param stream 入力元ストリーム
		* @return 成功時イメージのポインタ
		*
		* @detail 入力元ストリームが読み出し可能でなければならない
		*/
		static std::shared_ptr<Image> CreateFromStream(std::shared_ptr<IStream> stream);

		/**
		* @brief ブランク画像を作成する
		* @param width 横幅(1～65535)
		* @param height 縦幅(1～65535)
		* @return 成功時イメージのポインタ
		*/
		static std::shared_ptr<Image> CreateBlank(uint32_t width, uint32_t height);

		/**
		* @brief ストリームに対してイメージ(PNG)を出力する
		* @param stream 出力元ストリーム
		* @return 成否
		*
		* @detail 出力先ストリームが書き込み及びシーク可能でなければならない
		*/
		bool SaveToStream(std::shared_ptr<IStream> stream);

		/**
		* @brief イメージのサイズを変更する
		* @param width 変更後の横サイズ(1～65535)
		* @param height 変更後の縦サイズ(1～65535)
		* @return 成否
		*/
		bool Resize(uint32_t width, uint32_t height);

		/**
		* @brief イメージをコピーする
		* @param dstImage コピー先イメージ
		* @return 成否
		*
		* @detail イメージはコピー先イメージのサイズにリサイズされる
		*/
		bool CopyTo(std::shared_ptr<Image> dstImage);

		/**
		* @brief イメージの横幅を取得する
		* @return イメージの横幅
		*/
		uint32_t Width();

		/**
		* @brief イメージの縦幅を取得する
		* @return イメージの縦幅
		*/
		uint32_t Height();

		/**
		* @brief イメージの横幅をバイト数で取得する
		* @return イメージの横幅のバイト数
		*/
		uint32_t Pitch();

		/**
		* @brief イメージのデータの先頭ポインタを取得する
		* @return イメージへのポインタ
		*/
		Color* Data();

	protected:

		Image() {}

	private:

		class Impl;
		std::shared_ptr<Impl> m_pImpl;

		Image(const Image&) = delete;				// コピーコンストラクタ封じ
		Image& operator= (const Image&) = delete;	// コピーコンストラクタ封じ
	};
}

#endif
