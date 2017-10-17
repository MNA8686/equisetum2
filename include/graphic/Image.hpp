#if !defined(_EQIMAGE_H_)
#define _EQIMAGE_H_

#include "stream/IStream.h"
#include "type/Optional.h"
#include "type/Rect.hpp"
#include <memory>

#include "cereal/access.hpp"

namespace Equisetum2
{
	/**
	* カラークラス<br>
	* OpenGLやDirectX10は原則RGBAの並びなので、当ライブラリでもそれに従う。
	*/
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

	/**
	* イメージクラス
	*/
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
		virtual bool SaveToStream(std::shared_ptr<IStream> stream) const;

		/**
		* @brief イメージのサイズを変更する
		* @param width 変更後の横サイズ(1～65535)
		* @param height 変更後の縦サイズ(1～65535)
		* @return 成否
		*/
		virtual bool Resize(uint32_t width, uint32_t height);

		/**
		* @brief イメージをコピーする
		* @param dstImage コピー先イメージ
		* @param src コピー元イメージの範囲 省略時はコピー元イメージ全体
		* @param dst コピー先イメージの範囲 省略時はコピー先イメージ全体
		* @return 成否
		*/
		bool CopyTo(std::shared_ptr<Image> dstImage, const Optional<Rect> src= std::experimental::nullopt, const Optional<Rect> dst = std::experimental::nullopt) const;

		/**
		* @brief イメージの横幅を取得する
		* @return イメージの横幅
		*/
		virtual uint32_t Width() const;

		/**
		* @brief イメージの縦幅を取得する
		* @return イメージの縦幅
		*/
		virtual uint32_t Height() const;

		/**
		* @brief イメージの横幅をバイト数で取得する
		* @return イメージの横幅のバイト数
		*/
		virtual uint32_t Pitch() const;

		/**
		* @brief イメージのデータの先頭ポインタを取得する
		* @return イメージへのポインタ
		*/
		virtual Color* Data();

		/**
		* @brief イメージのデータの先頭ポインタを取得する
		* @return イメージへのポインタ
		*/
		virtual const Color* Data() const;

		/**
		* @brief 新しいインスタンスで現在のインスタンスを置き換える
		* @param src 置き換え元のインスタンス
		* @return 成否
		*/
		virtual bool MoveFrom(std::shared_ptr<Image>&& src);

		/**
		* @brief イメージのIDを設定する
		*/
		virtual void SetIdentify(const String& id);

		/**
		* @brief イメージのIDを取得する
		* @return イメージのID
		*/
		virtual String Identify() const;

		virtual ~Image() = default;

	protected:

		friend class cereal::access;
		Image() = default;

	private:

		class Impl;
		std::shared_ptr<Impl> m_pImpl;

		String m_id;		/// ID

		Image(const Image&) = delete;				// コピーコンストラクタ封じ
		Image& operator= (const Image&) = delete;	// コピーコンストラクタ封じ
		Image& operator= (const Image&&) = delete;	// ムーブセマンティクスコンストラクタ封じ
	};
}

#endif
