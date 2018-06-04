#include "system/Exception.hpp"
#include "system/Logger.h"

#include "graphic/RenderObjectImpl.hpp"

namespace Equisetum2
{
	RenderObject::~RenderObject()
	{
	}

	RenderObject::Type RenderObject::GetType() const
	{
		return m_type;
	}

	int RenderObject::GetLayer() const
	{
		return m_layer;
	}

	int RenderObject::GetOrderInLayer() const
	{
		return m_orderInLayer;
	}

	std::shared_ptr<SpriteRenderer> SpriteRenderer::Create()
	{
		EQ_DURING
		{
			// インスタンス作成
			auto inst = std::make_shared<SpriteRenderer>();
			if (!inst)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			// インスタンス初期化
			inst->m_pImpl = std::make_shared<SpriteRenderer::Impl>();
			if (!inst->m_pImpl)
			{
				EQ_THROW(u8"インスタンスの初期化に失敗しました。");
			}

			return inst;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return nullptr;
	}

	SpriteRenderer& SpriteRenderer::SetSprite(const std::shared_ptr<Sprite>& sprite)
	{
		m_sprite = sprite;
		return *this;
	}

	SpriteRenderer& SpriteRenderer::SetAtlasNum(int atlasNum)
	{
		m_atlasNum = atlasNum;
		return *this;
	}

	SpriteRenderer& SpriteRenderer::SetPos(Point pos)
	{
		m_pos = pos;
		return *this;
	}

	SpriteRenderer& SpriteRenderer::SetScale(float x, float y)
	{
		m_scale.x = x;
		m_scale.y = y;
		return *this;
	}

	SpriteRenderer& SpriteRenderer::SetScaleX(float x)
	{
		m_scale.x = x;
		return *this;
	}

	SpriteRenderer& SpriteRenderer::SetScaleY(float y)
	{
		m_scale.y = y;
		return *this;
	}

	SpriteRenderer& SpriteRenderer::SetColor(Color color)
	{
		m_color = color;
		return *this;
	}
	
	SpriteRenderer& SpriteRenderer::SetFlipX(bool isFlip)
	{
		m_flipX = isFlip;
		return *this;
	}

	SpriteRenderer& SpriteRenderer::SetFlipY(bool isFlip)
	{
		m_flipY = isFlip;
		return *this;
	}

	SpriteRenderer& SpriteRenderer::SetLayer(int layer)
	{
		m_layer = layer;
		return *this;
	}

	SpriteRenderer& SpriteRenderer::SetOrderInLayer(int32_t orderInLayer)
	{
		m_orderInLayer = orderInLayer;
		return *this;
	}

	SpriteRenderer& SpriteRenderer::SetBlendMode(Renderer::BlendMode blend)
	{
		m_blend = blend;
		return *this;
	}
}

