#include "system/Exception.hpp"
#include "system/Logger.h"

#include "graphic/RenderObjectImpl.hpp"

#include <cmath>

namespace Equisetum2
{
	static const float inv255f = 1.0f / 255.0f;

	RenderObject::~RenderObject()
	{
	}

	Type RenderObject::GetType() const
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

	std::shared_ptr<SpriteRenderer> SpriteRenderer::Create(std::shared_ptr<Renderer>& renderer)
	{
		EQ_DURING
		{
			// インスタンス作成
			auto inst = std::make_shared<SpriteRenderer>();
			if (!inst)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			// レンダラを保持
			inst->m_renderer = renderer;

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
		m_atlasNum = 0;

		m_dirtyTexCoords = true;
		return *this;
	}

	SpriteRenderer& SpriteRenderer::SetAtlasNum(int atlasNum)
	{
		m_atlasNum = atlasNum;

		m_dirtyTexCoords = true;
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

		m_dirtyColor = true;
		return *this;
	}
	
	SpriteRenderer& SpriteRenderer::SetAngle(float angle)
	{
		m_angle = fmod(angle, 360.f);
		m_angleRad = m_angle * 3.14159265358979323846f / 180.f;	// ラジアンを算出
		return *this;
	}

	SpriteRenderer& SpriteRenderer::SetFlipX(bool isFlip)
	{
		m_flipX = isFlip;

		m_dirtyTexCoords = true;
		return *this;
	}

	SpriteRenderer& SpriteRenderer::SetFlipY(bool isFlip)
	{
		m_flipY = isFlip;

		m_dirtyTexCoords = true;
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

	SpriteRenderer& SpriteRenderer::SetBlendMode(BlendMode blend)
	{
		m_blend = blend;
		return *this;
	}

	bool SpriteRenderer::Calculation()
	{
		auto atlas = m_sprite->GetAtlas(m_atlasNum);
		auto& vert = m_pImpl->m_vertex;

		// テクスチャ座標変更あり？
		if (m_dirtyTexCoords)
		{
			const float invWf = 1.0f / static_cast<float>(m_sprite->GetTexture()->Width());
			const float invHf = 1.0f / static_cast<float>(m_sprite->GetTexture()->Height());

			const float leftX = (m_flipX ? atlas.m_point.x + atlas.m_srcSize.x : atlas.m_point.x) * invWf;
			const float rightX = (m_flipX ? atlas.m_point.x : atlas.m_point.x + atlas.m_srcSize.x) * invWf;
			const float topY = (m_flipY ? atlas.m_point.y + atlas.m_srcSize.y : atlas.m_point.y) * invHf;
			const float bottomY = (m_flipY ? atlas.m_point.y : atlas.m_point.y + atlas.m_srcSize.y) * invHf;

			int index = 0;

			// 左上
			vert[index].texCoords[0] = leftX;
			vert[index].texCoords[1] = topY;
			index++;

			// 右上
			vert[index].texCoords[0] = rightX;
			vert[index].texCoords[1] = topY;
			index++;

			// 左下
			vert[index].texCoords[0] = leftX;
			vert[index].texCoords[1] = bottomY;
			index++;

			// 右下
			vert[index].texCoords[0] = rightX;
			vert[index].texCoords[1] = bottomY;

			m_dirtyTexCoords = false;
		}

		// カラー変更あり？
		if (m_dirtyColor)
		{
			const uint32_t color = m_color.pixel;

			for (auto& v : vert)
			{
				// 加減算する色
				auto p = reinterpret_cast<uint32_t*>(v.colors);
				*p = color;
			}

			m_dirtyColor = false;
		}

		// 転送先座標
		{
			// 基準点
			const auto baseX = static_cast<float>(m_pos.x);
			const auto baseY = static_cast<float>(m_pos.y);

			// 拡大縮小後のサイズ
			const float width = atlas.m_srcSize.x * m_scale.x;		// 拡大縮小後の横幅
			const float height = atlas.m_srcSize.y * m_scale.y;		// 拡大縮小後の縦幅

			// pivotで指定された表示位置になるようにする
			// (左上の座標を求める)
			auto& pivot = atlas.m_pivot;
			const float posX = baseX - pivot.x * width;
			const float posY = baseY - pivot.y * height;

			// 回転なし？
			if (m_angle == 0)
			{
				int index = 0;

				// 左上
				vert[index].vertices[0] = posX;
				vert[index].vertices[1] = posY;
				index++;

				// 右上
				vert[index].vertices[0] = posX + width;
				vert[index].vertices[1] = posY;
				index++;

				// 左下
				vert[index].vertices[0] = posX;
				vert[index].vertices[1] = posY + height;
				index++;

				// 右下
				vert[index].vertices[0] = posX + width;
				vert[index].vertices[1] = posY + height;
			}
			else
			{
				const auto& rad = m_angleRad;
				const auto cosVal = cos(rad);
				const auto sinVal = -sin(rad);

				for(int index=0; index < 4; index++)
				{
					float offsetX;
					float offsetY;

					// 原点からの相対座標を算出
					switch (index)
					{
					// 左上
					case 0:
						offsetX = posX - baseX;
						offsetY = posY - baseY;
						break;
					// 右上
					case 1:
						offsetX = (posX + width) - baseX;
						offsetY = posY - baseY;
						break;
					// 左下
					case 2:
						offsetX = posX - baseX;
						offsetY = (posY + height) - baseY;
						break;
					// 右下
					case 3:
						offsetX = (posX + width) - baseX;
						offsetY = (posY + height) - baseY;
						break;
					}

					// 回転する
					vert[index].vertices[0] = (offsetX * cosVal - offsetY * sinVal) + baseX;
					vert[index].vertices[1] = (offsetX * sinVal + offsetY * cosVal) + baseY;
				}
			}
		}

		return true;
	}
}

