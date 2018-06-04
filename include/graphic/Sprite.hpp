#if !defined(_EQSPRITE_H_)
#define _EQSPRITE_H_

//#include "Equisetum2.h"
#include "type/String.h"
#include "type/Point.h"
#include "type/Rect.hpp"
#include "graphic/Texture.hpp"
#include <memory>

namespace Equisetum2
{
	typedef struct
	{
	public:
		Point m_point;		// 開始座標
		Size m_srcSize;		// 転送元サイズ
		PointF m_pivot;		// 中心座標
	}stSpriteAnimAtlas;

	class Sprite
	{
	public:
		static const Color ZERO;

		Sprite() = default;
		virtual ~Sprite() = default;

		static std::shared_ptr<Sprite> CreateFromTexture(std::shared_ptr<Texture> texture);
		bool SetAnimAtlas(const std::vector<stSpriteAnimAtlas>& vAnimAtlas);

		const stSpriteAnimAtlas& GetAtlas(int32_t num);

		/**
		* @brief 新しいインスタンスで現在のインスタンスを置き換える
		* @param src 置き換え元のインスタンス
		* @return 成否
		*/
		virtual bool MoveFrom(std::shared_ptr<Sprite>&& src);

		/**
		* @brief イメージのIDを設定する
		*/
		virtual void SetIdentify(const String& id);

		/**
		* @brief イメージのIDを取得する
		* @return イメージのID
		*/
		virtual String Identify() const;

		const std::shared_ptr<Texture>& GetTexture() const;
		const std::vector<stSpriteAnimAtlas>& GetAnimAtlas() const;

	private:
		std::shared_ptr<Texture> m_texture;		/// 画像本体
		std::vector<stSpriteAnimAtlas> m_vAnimAtlas;	/// 切り出し位置

		String m_id;		/// SpriteのID
	};
}

#endif
