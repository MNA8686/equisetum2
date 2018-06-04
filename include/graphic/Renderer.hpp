#if !defined(_EQRENDERER_H_)
#define _EQRENDERER_H_

#include "type/String.h"
#include "type/Point.h"
#include "type/Rect.hpp"
#include "graphic/Image.hpp"
#include "graphic/Texture.hpp"
#include "graphic/Sprite.hpp"
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
		Type GetType();

	private:
		Type m_type = Type::EMPTY;
	};

	class SpriteRenderer : public RenderObject
	{
	public:
		SpriteRenderer();
		virtual ~SpriteRenderer();

		SpriteRenderer& SetSprite(const std::shared_ptr<Sprite>& sprite, int atlasNum);

		SpriteRenderer& SetPos(int x, int y);

		SpriteRenderer& SetScale(float x, float y);
		SpriteRenderer& SetScaleX(float x);
		SpriteRenderer& SetScaleY(float y);
		SpriteRenderer& SetColor(Color color);

		SpriteRenderer& SetFlipX(bool isFlip);
		SpriteRenderer& SetFlipY(bool isFlip);

		SpriteRenderer& SetLayer(int layer);
		SpriteRenderer& SetOrderInLayer(int orderInLayer);

		SpriteRenderer& SetBlendMode(int blend);
	};


	/**
	* レンダラクラス
	*/
	class Renderer
	{
	public:

		class BlendMode
		{
		public:
			static const int None = 0;
			static const int Blend = (1 << 0);
			static const int Add = (1 << 1);
		};

		virtual ~Renderer() = default;

		static std::shared_ptr<Renderer> Create();

		bool AddCommandQueue(const std::shared_ptr<Texture>& tex, const Rect& dst, const Rect& src, int flag, int layer, Color color = { 128, 128, 128, 128 });


		bool Copy(const std::shared_ptr<Texture>& tex, const Rect& dst, const Rect& src, int flag, int layer);
		bool CopyWithColor(const std::shared_ptr<Texture>& tex, const Rect& dst, const Rect& src, int flag, int layer, Color color);
		bool Fill(const Rect& dst, int flag, int layer, Color color);

		bool Clear(const Color& color);
		bool Present(bool waitVsync = true);

		Renderer() = default;

	private:

		class Impl;
		std::shared_ptr<Impl> m_pImpl;

		String m_id;		/// ID

		Renderer(const Renderer&) = delete;					// コピーコンストラクタ封じ
		Renderer& operator= (const Renderer&) = delete;		// コピーコンストラクタ封じ
		Renderer& operator= (const Renderer&&) = delete;	// ムーブセマンティクスコンストラクタ封じ
	};
}

#endif
