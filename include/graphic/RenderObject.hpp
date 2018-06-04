#if !defined(_EQRENDEROBJECT_H_)
#define _EQRENDEROBJECT_H_

#include "type/String.h"
#include "type/Point.h"
#include "type/Rect.hpp"
#include "graphic/Image.hpp"
#include "graphic/Texture.hpp"
#include "graphic/Renderer.hpp"
#include <memory>

namespace Equisetum2
{
	class RenderObject
	{
	public:
		enum class Type : int
		{
			EMPTY = 0,
			POINT,
			LINE,
			RECT,
			CIRCLE,
			SPRITE,
		};

		virtual ~RenderObject();
		Type GetType() const;
		int GetLayer() const;
		int32_t GetOrderInLayer() const;

	protected:
		Type m_type = Type::EMPTY;
		int m_layer = 0;			/// 表示レイヤー
		int32_t m_orderInLayer = 0;		/// レイヤー内での表示順序(小さいほど奥に表示される)
	};

	class Sprite;
	class SpriteRenderer : public RenderObject
	{
	public:
		std::shared_ptr<SpriteRenderer> Create();

		SpriteRenderer() = default;
		virtual ~SpriteRenderer() = default;

		SpriteRenderer& SetSprite(const std::shared_ptr<Sprite>& sprite);
		SpriteRenderer& SetAtlasNum(int atlasNum);

		SpriteRenderer& SetPos(Point pos);

		SpriteRenderer& SetScale(float x, float y);
		SpriteRenderer& SetScaleX(float x);
		SpriteRenderer& SetScaleY(float y);
		SpriteRenderer& SetColor(Color color);

		SpriteRenderer& SetFlipX(bool isFlip);
		SpriteRenderer& SetFlipY(bool isFlip);

		SpriteRenderer& SetLayer(int layer);
		SpriteRenderer& SetOrderInLayer(int32_t orderInLayer);

		SpriteRenderer& SetBlendMode(Renderer::BlendMode blend);

	private:
		std::shared_ptr<Sprite> m_sprite;	/// 表示スプライト
		int m_atlasNum = 0;		/// 表示パターン番号

		Point m_pos;			/// 表示位置
		SizeF m_scale{ 1.f, 1.f };	/// 表示倍率
		Color m_color = Sprite::ZERO;	/// 表示色
		Renderer::BlendMode m_blend = Renderer::BlendMode::None;	/// ブレンドモード
		bool m_flipX = false;	/// X方向反転
		bool m_flipY = false;	/// Y方向反転

		class Impl;
		std::shared_ptr<Impl> m_pImpl;
	};
}

#endif
