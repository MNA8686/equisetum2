﻿#if !defined(_EQRENDEROBJECT_H_)
#define _EQRENDEROBJECT_H_

#include "type/String.h"
#include "type/Point.h"
#include "type/Rect.hpp"
#include "graphic/Image.hpp"
#include "graphic/Texture.hpp"
#include "graphic/Sprite.hpp"
#include "graphic/RendererState.hpp"
#include "graphic/RenderObject.hpp"
#include "graphic/Renderer.hpp"
#include "util/AssetManager.hpp"
#include <memory>

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
//#include <cereal/types/base_class.hpp>

//using namespace Equisetum2;
using namespace Equisetum2::RenderState;

namespace Equisetum2
{
	class Renderer;
	class RenderObject
	{
	public:
		virtual ~RenderObject();
		Type GetType() const;
		int GetLayer() const;
		int32_t GetOrderInLayer() const;
		bool IsVisible() const;
		void SetVisible(bool visible);

#if 1
		template<class Archive>
		void save(Archive & archive) const
		{
			archive(CEREAL_NVP(m_type));
			archive(CEREAL_NVP(m_layer));
			archive(CEREAL_NVP(m_orderInLayer));
			archive(CEREAL_NVP(m_visible));
			//archive(CEREAL_NVP(m_renderer));
		}

		template<class Archive>
		void load(Archive & archive)
		{
			archive(CEREAL_NVP(m_type));
			archive(CEREAL_NVP(m_layer));
			archive(CEREAL_NVP(m_orderInLayer));
			archive(CEREAL_NVP(m_visible));
			//archive(CEREAL_NVP(m_renderer));
		}
#endif

	protected:
		// --- serialize begin ---
		Type m_type = Type::EMPTY;
		int m_layer = 0;			/// 表示レイヤー
		int32_t m_orderInLayer = 0;		/// レイヤー内での表示順序(小さいほど奥に表示される)
		bool m_visible = true;		/// 表示属性
		std::weak_ptr<Renderer> m_renderer;
		// --- serialize end ---
	};
}

namespace Equisetum2
{
	class SpriteRenderer : public RenderObject
	{
	public:
		SpriteRenderer() { m_type = Type::SPRITE; }
		virtual ~SpriteRenderer() {}

		SpriteRenderer& SetSprite(const std::shared_ptr<Sprite>& sprite);
		SpriteRenderer& SetAtlasNum(int atlasNum);
		
		SpriteRenderer& SetPos(const Point& pos);

		SpriteRenderer& SetScale(float x, float y);
		SpriteRenderer& SetScaleX(float x);
		SpriteRenderer& SetScaleY(float y);
		SpriteRenderer& SetColor(const Color& color);
		SpriteRenderer& SetAngle(float angle);

		SpriteRenderer& SetFlipX(bool isFlip);
		SpriteRenderer& SetFlipY(bool isFlip);

		SpriteRenderer& SetLayer(int layer);
		SpriteRenderer& SetOrderInLayer(int32_t orderInLayer);

		SpriteRenderer& SetBlendMode(BlendMode blend);

		bool Calculation();

		template<class Archive>
		void save(Archive & archive) const 
		{
			archive(cereal::base_class<RenderObject>(this));

			std::string spriteId = m_sprite->Identify();
			archive(CEREAL_NVP(spriteId));

			archive(CEREAL_NVP(m_atlasNum));
			archive(CEREAL_NVP(m_pos));
			archive(CEREAL_NVP(m_scale));
			archive(CEREAL_NVP(m_color.pixel));
			archive(CEREAL_NVP(m_blend));
			archive(CEREAL_NVP(m_flipX));
			archive(CEREAL_NVP(m_flipY));
			archive(CEREAL_NVP(m_angle));
		}

		template<class Archive>
		void load(Archive & archive)
		{
			archive(cereal::base_class<RenderObject>(this));
			InitTest();

			std::string spriteId;
			archive(CEREAL_NVP(spriteId));
			m_sprite = Singleton<AssetManager>::GetInstance()->Load<Sprite>(spriteId);

			archive(CEREAL_NVP(m_atlasNum));
			archive(CEREAL_NVP(m_pos));
			archive(CEREAL_NVP(m_scale));
			archive(CEREAL_NVP(m_color.pixel));
			archive(CEREAL_NVP(m_blend));
			archive(CEREAL_NVP(m_flipX));
			archive(CEREAL_NVP(m_flipY));
			archive(CEREAL_NVP(m_angle));
		}

	private:
		friend class Renderer;
		static std::shared_ptr<SpriteRenderer> Create(std::shared_ptr<Renderer>& renderer);
		void InitTest();
		//
		std::shared_ptr<Sprite> m_sprite;	/// 表示スプライト
		int m_atlasNum = 0;		/// 表示パターン番号
		Point m_pos;			/// 表示位置
		SizeF m_scale{ 1.f, 1.f };	/// 表示倍率
		Color m_color = Sprite::ZERO;	/// 表示色
		BlendMode m_blend = BlendMode::None;	/// ブレンドモード
		bool m_flipX = false;	/// X方向反転
		bool m_flipY = false;	/// Y方向反転
		float m_angle = 0;		/// 回転角度
		//

		float m_angleRad = 0;
		bool m_dirtyTexCoords = true;
		bool m_dirtyColor = true;

		class Impl;
		std::shared_ptr<Impl> m_pImpl;
	};
}

#include <cereal/types/polymorphic.hpp>
CEREAL_REGISTER_TYPE(Equisetum2::SpriteRenderer);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Equisetum2::RenderObject, Equisetum2::SpriteRenderer)

namespace Equisetum2
{
	class LineRenderer : public RenderObject
	{
	public:
		LineRenderer() { m_type = Type::LINE; }
		virtual ~LineRenderer() {}

		LineRenderer& Clear();
		LineRenderer& PushLine(const Point& m_beginPos, const Point& m_endPos);

		LineRenderer& SetColor(const Color& color);

		LineRenderer& SetLayer(int layer);
		LineRenderer& SetOrderInLayer(int32_t orderInLayer);

		LineRenderer& SetBlendMode(BlendMode blend);

		bool Calculation();

		template<class Archive>
		void save(Archive & archive) const
		{
			archive(cereal::base_class<RenderObject>(this));
		}

		template<class Archive>
		void load(Archive & archive)
		{
			archive(cereal::base_class<RenderObject>(this));
			InitTest();
		}

	private:
		friend class Renderer;
		static std::shared_ptr<LineRenderer> Create(std::shared_ptr<Renderer>& renderer);
		void InitTest();

		std::vector<Point> m_vPos;			/// 表示位置
		Color m_color = Sprite::ZERO;	/// 表示色
		BlendMode m_blend = BlendMode::None;	/// ブレンドモード

		class Impl;
		std::shared_ptr<Impl> m_pImpl;
	};
}

#include <cereal/types/polymorphic.hpp>
CEREAL_REGISTER_TYPE(Equisetum2::LineRenderer);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Equisetum2::RenderObject, Equisetum2::LineRenderer)

#endif
