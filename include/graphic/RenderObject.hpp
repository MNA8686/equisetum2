#if !defined(_EQRENDEROBJECT_H_)
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
#include "graphic/BitmapFont.hpp"
#include "util/AssetManager.hpp"
#include <memory>

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>

using namespace Equisetum2::RenderState;

namespace Equisetum2
{
	class Renderer;
	class RenderObject
	{
	public:
		virtual ~RenderObject();
		RenderType GetType() const;
		int32_t GetSubType() const;
		int GetLayer() const;
		int32_t GetOrderInLayer() const;
		bool IsVisible() const;
		void SetVisible(bool visible);

#if 1
		template<class Archive>
		void save(Archive & archive) const
		{
			archive(CEREAL_NVP(m_type));
			archive(CEREAL_NVP(m_subType));
			archive(CEREAL_NVP(m_layer));
			archive(CEREAL_NVP(m_orderInLayer));
			archive(CEREAL_NVP(m_visible));
		}

		template<class Archive>
		void load(Archive & archive)
		{
			archive(CEREAL_NVP(m_type));
			archive(CEREAL_NVP(m_subType));
			archive(CEREAL_NVP(m_layer));
			archive(CEREAL_NVP(m_orderInLayer));
			archive(CEREAL_NVP(m_visible));
		}

		virtual bool Calculation() = 0;
#endif

	protected:
		// --- serialize begin ---
		RenderType m_type = RenderType::EMPTY;
		int32_t m_subType = 0;
		int m_layer = 0;			/// 表示レイヤー
		int32_t m_orderInLayer = 0;		/// レイヤー内での表示順序(小さいほど奥に表示される)
		bool m_visible = true;		/// 表示属性
		// --- serialize end ---
	};
}

namespace Equisetum2
{
	class SpriteRenderer : public RenderObject
	{
	public:
		SpriteRenderer() { m_type = RenderType::SPRITE; }
		virtual ~SpriteRenderer() {}

		SpriteRenderer& SetSprite(const std::shared_ptr<Sprite>& sprite);
		const Sprite* GetSprite() const;
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
			archive(CEREAL_NVP(m_color));
			archive(CEREAL_NVP(m_blend));
			archive(CEREAL_NVP(m_flipX));
			archive(CEREAL_NVP(m_flipY));
			archive(CEREAL_NVP(m_angle));
		}

		template<class Archive>
		void load(Archive & archive)
		{
			InitTest();
			archive(cereal::base_class<RenderObject>(this));

			std::string spriteId;
			archive(CEREAL_NVP(spriteId));
			m_sprite = Singleton<AssetManager>::GetInstance()->Load<Sprite>(spriteId);

			archive(CEREAL_NVP(m_atlasNum));
			archive(CEREAL_NVP(m_pos));
			archive(CEREAL_NVP(m_scale));
			archive(CEREAL_NVP(m_color));
			archive(CEREAL_NVP(m_blend));
			archive(CEREAL_NVP(m_flipX));
			archive(CEREAL_NVP(m_flipY));
			archive(CEREAL_NVP(m_angle));
		}

		class Impl;
		std::shared_ptr<Impl> m_pImpl;

	private:
		friend class Renderer;
		static std::shared_ptr<SpriteRenderer> Create();
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
	};
}

#include <cereal/types/polymorphic.hpp>
CEREAL_REGISTER_TYPE(Equisetum2::SpriteRenderer);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Equisetum2::RenderObject, Equisetum2::SpriteRenderer)

namespace Equisetum2
{
	enum class TextHAlignment  : int
	{
		Left,
		Center,
		Right
	};

	class TextRenderer : public RenderObject
	{
	public:
		TextRenderer() { m_type = RenderType::TEXT; }
		virtual ~TextRenderer() {}

		TextRenderer& SetBitmapFont(const std::shared_ptr<BitmapFont>& bitmapFont);
		TextRenderer& SetText(const String& text);

		TextRenderer& SetPos(const Point& pos);

		TextRenderer& SetScale(float x, float y);
		TextRenderer& SetScaleX(float x);
		TextRenderer& SetScaleY(float y);
		TextRenderer& SetColor(const Color& color);
		TextRenderer& SetAngle(float angle);

		TextRenderer& SetFlipX(bool isFlip);
		TextRenderer& SetFlipY(bool isFlip);
		TextRenderer& SetTextHAlignment(TextHAlignment textHAlignment);
		TextRenderer& SetPivot(PointF pivot);

		TextRenderer& SetLayer(int layer);
		TextRenderer& SetOrderInLayer(int32_t orderInLayer);

		TextRenderer& SetBlendMode(BlendMode blend);

		std::shared_ptr<SpriteRenderer> GetLetter(size_t letterNum);
		size_t GetLetterSize() const;
		Size GetBoxSize() const;
		const std::u32string& GetTextU32() const;

		bool Calculation();

		template<class Archive>
		void save(Archive & archive) const
		{
			archive(cereal::base_class<RenderObject>(this));

			archive(CEREAL_NVP(m_pos));
			archive(CEREAL_NVP(m_scale));
			archive(CEREAL_NVP(m_pivot));
			archive(CEREAL_NVP(m_color));
			archive(CEREAL_NVP(m_blend));
			archive(CEREAL_NVP(m_flipX));
			archive(CEREAL_NVP(m_flipY));
			archive(CEREAL_NVP(m_angle));
			archive(CEREAL_NVP(m_textHAlignment));
			std::string text = String(m_text);
			archive(CEREAL_NVP(text));

			BitmapFont::SerializeHint::From from = BitmapFont::SerializeHint::From::Empty;

			if (m_bitmapFont)
			{
				BitmapFont::SerializeHint hint = m_bitmapFont->GetHint();
				from = hint.from;

				if (from == BitmapFont::SerializeHint::From::FontManager)
				{
					std::string id = hint.id;
					archive(CEREAL_NVP(id));
					Color color = hint.color;
					archive(CEREAL_NVP(color));
					Size maxSize = hint.maxSize;
					archive(CEREAL_NVP(maxSize));
					std::string codepoint = m_bitmapFont->CodePoint();
					archive(CEREAL_NVP(codepoint));
				}
				else if (from == BitmapFont::SerializeHint::From::Asset)
				{
					std::string id = hint.id;
					archive(CEREAL_NVP(id));
				}
			}
			archive(CEREAL_NVP(from));
		}

		template<class Archive>
		void load(Archive & archive)
		{
			InitTest();
			archive(cereal::base_class<RenderObject>(this));
			archive(CEREAL_NVP(m_pos));
			archive(CEREAL_NVP(m_scale));
			archive(CEREAL_NVP(m_pivot));
			archive(CEREAL_NVP(m_color));
			archive(CEREAL_NVP(m_blend));
			archive(CEREAL_NVP(m_flipX));
			archive(CEREAL_NVP(m_flipY));
			archive(CEREAL_NVP(m_angle));
			archive(CEREAL_NVP(m_textHAlignment));
			std::string text;
			archive(CEREAL_NVP(text));

			BitmapFont::SerializeHint::From from = BitmapFont::SerializeHint::From::Empty;
			archive(CEREAL_NVP(from));

			if (from == BitmapFont::SerializeHint::From::FontManager)
			{
				std::string id;
				archive(CEREAL_NVP(id));

				if (!id.empty())
				{
					auto font = Singleton<AssetManager>::GetInstance()->Load<FontManager>(id);
					if (font)
					{
						Color color;
						archive(CEREAL_NVP(color));
						Size maxSize;
						archive(CEREAL_NVP(maxSize));
						std::string codepoint;
						archive(CEREAL_NVP(codepoint));

						SetBitmapFont(font->MakeBitmapFont(codepoint, color, maxSize));
						SetText(text);
					}
				}
			}
			else if (from == BitmapFont::SerializeHint::From::Asset)
			{
				std::string id;
				archive(CEREAL_NVP(id));

				if (!id.empty())
				{
					auto bitmapfont = Singleton<AssetManager>::GetInstance()->Load<BitmapFont>(id);
					if (bitmapfont)
					{
						SetBitmapFont(bitmapfont);
						SetText(text);
					}
				}
			}
		}

		class Impl;
		std::shared_ptr<Impl> m_pImpl;

	private:
		friend class Renderer;
		static std::shared_ptr<TextRenderer> Create();
		void InitTest();

		void MeasurementBoxSize();

		//
		std::shared_ptr<BitmapFont> m_bitmapFont;
		Point m_pos;			/// 表示位置
		SizeF m_scale{ 1.f, 1.f };	/// 表示倍率
		PointF m_pivot{ 0, 0 };	/// ピボット
		Color m_color = Sprite::ZERO;	/// 表示色
		BlendMode m_blend = BlendMode::None;	/// ブレンドモード
		bool m_flipX = false;	/// X方向反転
		bool m_flipY = false;	/// Y方向反転
		float m_angle = 0;		/// 回転角度
		TextHAlignment m_textHAlignment = TextHAlignment::Left;
		std::u32string m_text;		/// コードポイント配列
		//

		float m_angleRad = 0;
		std::vector<std::shared_ptr<SpriteRenderer>> m_vSpriteRenderer;
		int32_t m_height = 0;		/// フォントの高さ

		typedef struct
		{
			int32_t x = -1;
			int atlas = -1;
		}stCodeMap;

		std::vector<stCodeMap> m_vCodeMap;
		std::vector<int> m_vWidth;	/// 行ごとの横幅
		Size m_boxSize;		/// 文字列が収まるボックスのサイズ
	};
}

#include <cereal/types/polymorphic.hpp>
CEREAL_REGISTER_TYPE(Equisetum2::TextRenderer);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Equisetum2::RenderObject, Equisetum2::TextRenderer)

namespace Equisetum2
{
	class PrimitiveRenderer : public RenderObject
	{
	public:
		PrimitiveRenderer() { m_type = RenderType::PRIMITIVE; }
		virtual ~PrimitiveRenderer() {}

		template<class Archive>
		void save(Archive & archive) const
		{
			archive(cereal::base_class<RenderObject>(this));
		}

		template<class Archive>
		void load(Archive & archive)
		{
			archive(cereal::base_class<RenderObject>(this));
		}

		class Impl;
		std::shared_ptr<Impl> m_pImpl;
	};
}

#include <cereal/types/polymorphic.hpp>
CEREAL_REGISTER_TYPE(Equisetum2::PrimitiveRenderer);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Equisetum2::RenderObject, Equisetum2::PrimitiveRenderer)

namespace Equisetum2
{
	class LineRenderer : public PrimitiveRenderer
	{
	public:
		LineRenderer() { m_subType = PrimitiveType::LINE; }
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
			archive(cereal::base_class<PrimitiveRenderer>(this));

			archive(CEREAL_NVP(m_vPos));
			archive(CEREAL_NVP(m_color));
			archive(CEREAL_NVP(m_blend));
			archive(CEREAL_NVP(m_vPosSize));
		}

		template<class Archive>
		void load(Archive & archive)
		{
			InitTest();
			archive(cereal::base_class<PrimitiveRenderer>(this));

			archive(CEREAL_NVP(m_vPos));
			archive(CEREAL_NVP(m_color));
			archive(CEREAL_NVP(m_blend));
			archive(CEREAL_NVP(m_vPosSize));
		}

	private:
		friend class Renderer;
		static std::shared_ptr<LineRenderer> Create();
		void InitTest();

		std::vector<Point> m_vPos;			/// 表示位置
		Color m_color = ColorDef::White;	/// 表示色
		BlendMode m_blend = BlendMode::None;	/// ブレンドモード
		int32_t m_vPosSize = 0;		/// vectorにpush_backするとdebug不能な遅さになるため、vectorはただの可変長配列として扱う
	};
}

#include <cereal/types/polymorphic.hpp>
CEREAL_REGISTER_TYPE(Equisetum2::LineRenderer);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Equisetum2::PrimitiveRenderer, Equisetum2::LineRenderer)

namespace Equisetum2
{
	class RectRenderer : public PrimitiveRenderer
	{
	public:
		RectRenderer() { m_subType = PrimitiveType::RECT; }
		virtual ~RectRenderer() {}

		RectRenderer& SetRect(const Rect& rect, bool solid = true);

		RectRenderer& SetColor(const Color& color);
		RectRenderer& SetColor(const Color& leftTop, const Color& rightTop, const Color& leftBottom, const Color& rightBottom);

		RectRenderer& SetLayer(int layer);
		RectRenderer& SetOrderInLayer(int32_t orderInLayer);

		RectRenderer& SetBlendMode(BlendMode blend);

		bool Calculation();

		template<class Archive>
		void save(Archive & archive) const
		{
			archive(cereal::base_class<PrimitiveRenderer>(this));

			archive(CEREAL_NVP(m_rect));
			archive(CEREAL_NVP(m_colors));
			archive(CEREAL_NVP(m_blend));
			archive(CEREAL_NVP(m_solid));
		}

		template<class Archive>
		void load(Archive & archive)
		{
			InitTest();
			archive(cereal::base_class<PrimitiveRenderer>(this));

			archive(CEREAL_NVP(m_rect));
			archive(CEREAL_NVP(m_colors));
			archive(CEREAL_NVP(m_blend));
			archive(CEREAL_NVP(m_solid));
		}

	private:
		friend class Renderer;
		static std::shared_ptr<RectRenderer> Create();
		void InitTest();

		static const int vertexMax = 4;

		Rect m_rect{};			/// 表示位置
		Color m_colors[vertexMax]{ ColorDef::White, ColorDef::White, ColorDef::White, ColorDef::White };	/// 表示色
		BlendMode m_blend = BlendMode::None;	/// ブレンドモード
		bool m_solid = false;		/// 塗りつぶしフラグ
	};
}

#include <cereal/types/polymorphic.hpp>
CEREAL_REGISTER_TYPE(Equisetum2::RectRenderer);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Equisetum2::PrimitiveRenderer, Equisetum2::RectRenderer)

namespace Equisetum2
{
	class CircleRenderer : public PrimitiveRenderer
	{
	public:
		CircleRenderer() { m_subType = PrimitiveType::CIRCLE; }
		virtual ~CircleRenderer() {}

		CircleRenderer& SetCircle(const Point& centerPos, int32_t radius, bool solid=true);

		CircleRenderer& SetColor(const Color& color);

		CircleRenderer& SetLayer(int layer);
		CircleRenderer& SetOrderInLayer(int32_t orderInLayer);

		CircleRenderer& SetBlendMode(BlendMode blend);

		bool Calculation();

		template<class Archive>
		void save(Archive & archive) const
		{
			archive(cereal::base_class<PrimitiveRenderer>(this));

			archive(CEREAL_NVP(m_pos));
			archive(CEREAL_NVP(m_radius));
			archive(CEREAL_NVP(m_color));
			archive(CEREAL_NVP(m_blend));
			archive(CEREAL_NVP(m_solid));
		}

		template<class Archive>
		void load(Archive & archive)
		{
			InitTest();
			archive(cereal::base_class<PrimitiveRenderer>(this));

			archive(CEREAL_NVP(m_pos));
			archive(CEREAL_NVP(m_radius));
			archive(CEREAL_NVP(m_color));
			archive(CEREAL_NVP(m_blend));
			archive(CEREAL_NVP(m_solid));
		}

	private:
		friend class Renderer;
		static std::shared_ptr<CircleRenderer> Create();
		void InitTest();

		Point m_pos{};			/// 表示位置
		int32_t m_radius = 0;		/// 半径
		Color m_color = ColorDef::White;	/// 表示色
		BlendMode m_blend = BlendMode::None;	/// ブレンドモード
		bool m_solid = false;		/// 塗りつぶしフラグ
	};
}

#include <cereal/types/polymorphic.hpp>
CEREAL_REGISTER_TYPE(Equisetum2::CircleRenderer);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Equisetum2::PrimitiveRenderer, Equisetum2::CircleRenderer)

#endif
