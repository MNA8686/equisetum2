#if !defined(_EQTEXTURE_H_)
#define _EQTEXTURE_H_

#include "type/String.h"
#include "type/Point.h"
#include "graphic/Image.hpp"
#include <memory>

namespace Equisetum2
{
	class Texture
	{
	public:
		class AccessFlag
		{
		public:
			static const int Static       = (1 << 0);
			static const int Stream       = (1 << 1);		// don't work
			static const int RenderTarget = (1 << 2);
		};

		static std::shared_ptr<Texture> CreateFromImage(const std::shared_ptr<Image> image);
		static std::shared_ptr<Texture> CreateBlank(uint32_t width, uint32_t height, int32_t flag);
		virtual bool Update(const Point& pos, const std::shared_ptr<Image> image);

		/**
		* @brief テクスチャの横幅を取得する
		* @return テクスチャの横幅
		*/
		virtual uint32_t Width() const;

		/**
		* @brief テクスチャの縦幅を取得する
		* @return テクスチャの縦幅
		*/
		virtual uint32_t Height() const;

		/**
		* @brief 新しいインスタンスで現在のインスタンスを置き換える
		* @param src 置き換え元のインスタンス
		* @return 成否
		*/
		virtual bool MoveFrom(std::shared_ptr<Texture>&& src);

		/**
		* @brief イメージのIDを設定する
		*/
		virtual void SetIdentify(const String& id);

		/**
		* @brief イメージのIDを取得する
		* @return イメージのID
		*/
		virtual String Identify() const;

		class Impl;
		std::shared_ptr<Impl> m_pImpl;

	protected:

		Texture() = default;

	private:

		String m_id;		/// ID
		int32_t m_flag = AccessFlag::Static;		/// テクスチャアクセスフラグ
	
		Texture(const Texture&) = delete;				// コピーコンストラクタ封じ
		Texture& operator= (const Texture&) = delete;	// コピーコンストラクタ封じ
		Texture& operator= (const Texture&&) = delete;	// ムーブセマンティクスコンストラクタ封じ
	};

}

#endif
