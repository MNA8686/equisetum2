#if !defined(_EQIMAGE_H_)
#define _EQIMAGE_H_

#include "stream/IStream.h"
#include <memory>

namespace Equisetum2
{
	union Color
	{
		struct rgba
		{
			uint8_t a;
			uint8_t r;
			uint8_t g;
			uint8_t b;
		};

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
		* @brief ストリームに対してイメージ(PNG)を出力する
		* @param stream 出力元ストリーム
		* @return 成否
		*
		* @detail 出力先ストリームが書き込み及びシーク可能でなければならない
		*/
		bool SaveToStream(std::shared_ptr<IStream> stream);

		/**
		* @brief イメージのサイズを変更する
		* @param width 変更後の横サイズ
		* @param height 変更後の縦サイズ
		* @return 成否
		*/
		bool Resize(uint32_t width, uint32_t height);

		/**
		* @brief イメージの横幅を取得する
		* @return 成否
		*/
		uint32_t Width();

		/**
		* @brief イメージの縦幅を取得する
		* @return 成否
		*/
		uint32_t Height();

		/**
		* @brief イメージの横幅をバイト数で取得する
		* @return 成否
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

		bool InitFromStream(std::shared_ptr<IStream>& stream);
	};
}

#endif
