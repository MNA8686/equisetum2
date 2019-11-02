#include "system/Exception.hpp"
#include "system/Logger.h"

#include "graphic/RenderObjectImpl.hpp"

#include <cmath>

namespace std {
	template <class T>
	constexpr const T& clamp(const T& v, const T& low, const T& high)
	{
		return min(max(v, low), high);
	}
}

namespace Equisetum2
{
	static const float inv255f = 1.0f / 255.0f;

	RenderObject::~RenderObject()
	{
	}

	RenderType RenderObject::GetType() const
	{
		return m_type;
	}

	int32_t RenderObject::GetSubType() const
	{
		return m_subType;
	}

	int RenderObject::GetLayer() const
	{
		return m_layer;
	}

	int RenderObject::GetOrderInLayer() const
	{
		return m_orderInLayer;
	}

	bool RenderObject::IsVisible() const
	{
		return m_visible;
	}

	void RenderObject::SetVisible(bool visible)
	{
		m_visible = visible;
	}

	void SpriteRenderer::InitTest()
	{
		m_pImpl = std::make_shared<SpriteRenderer::Impl>();
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
		if (m_sprite != sprite)
		{
			m_sprite = sprite;
		}
		m_atlasNum = 0;

		m_dirtyTexCoords = true;
		return *this;
	}

	const Sprite* SpriteRenderer::GetSprite() const
	{
		return m_sprite.get();
	}

	SpriteRenderer& SpriteRenderer::SetAtlasNum(int atlasNum)
	{
		m_atlasNum = atlasNum;

		m_dirtyTexCoords = true;
		return *this;
	}

	SpriteRenderer& SpriteRenderer::SetPos(const Point& pos)
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

	SpriteRenderer& SpriteRenderer::SetColor(const Color& color)
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
		if (!m_sprite)
		{
			return false;
		}
		
		const stSpriteAnimAtlas* atlas = m_sprite->GetAtlas(m_atlasNum);
		auto& vert = m_pImpl->m_vertex;

		if (!atlas)
		{
			return false;
		}

		// テクスチャ座標変更あり？
		if (m_dirtyTexCoords)
		{
			const float invWf = 1.0f / static_cast<float>(m_sprite->GetTexture()->Width());
			const float invHf = 1.0f / static_cast<float>(m_sprite->GetTexture()->Height());

			const float leftX = (m_flipX ? atlas->m_point.x + atlas->m_srcSize.x : atlas->m_point.x) * invWf;
			const float rightX = (m_flipX ? atlas->m_point.x : atlas->m_point.x + atlas->m_srcSize.x) * invWf;
			const float topY = (m_flipY ? atlas->m_point.y + atlas->m_srcSize.y : atlas->m_point.y) * invHf;
			const float bottomY = (m_flipY ? atlas->m_point.y : atlas->m_point.y + atlas->m_srcSize.y) * invHf;

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
			const float width = atlas->m_srcSize.x * m_scale.x;		// 拡大縮小後の横幅
			const float height = atlas->m_srcSize.y * m_scale.y;		// 拡大縮小後の縦幅

			// pivotで指定された表示位置になるようにする
			// (左上の座標を求める)
			auto& pivot = atlas->m_pivot;
			const float posX = floor(baseX - pivot.x * width);
			const float posY = floor(baseY - pivot.y * height);

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

				for (int index = 0; index < 4; index++)
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





	void TextRenderer::InitTest()
	{
		m_pImpl = std::make_shared<TextRenderer::Impl>();
	}

	std::shared_ptr<TextRenderer> TextRenderer::Create()
	{
		EQ_DURING
		{
			// インスタンス作成
			auto inst = std::make_shared<TextRenderer>();
			if (!inst)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			// インスタンス初期化
			inst->m_pImpl = std::make_shared<TextRenderer::Impl>();
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

	TextRenderer& TextRenderer::SetBitmapFont(const std::shared_ptr<BitmapFont>& bitmapFont)
	{
		if (m_bitmapFont != bitmapFont)
		{
			m_bitmapFont = bitmapFont;

			if (m_bitmapFont)
			{
				// BitmapFont作成時に高さが統一されていることが保証されているのでそのまま代入
				m_height = m_bitmapFont->GetSprite()->GetAtlas(0)->m_srcSize.y;

				for (auto& sprite : m_vSpriteRenderer)
				{
					sprite->SetSprite(m_bitmapFont->GetSprite());
				}
			}

			MeasurementBoxSize();
		}

		return *this;
	}

	TextRenderer& TextRenderer::SetText(const String& text)
	{
		m_text = text.to_u32();

		m_vSpriteRenderer.resize(m_text.size());
		m_visibleSize = static_cast<int32_t>(m_text.size());

		if (auto pRenderer = Singleton<CurrentRenderer>::GetInstance()->Get())
		{
			// 1文字1スプライトとして運用する
			for (uint32_t i = 0; i < m_text.size(); i++)
			{
				auto& spriteRenderer = m_vSpriteRenderer[i];

				if (!spriteRenderer)
				{
					// インスタンス作成
					spriteRenderer = pRenderer->CreateRenderObject<SpriteRenderer>();
				}

				if (m_bitmapFont)
				{
					// spriteRendererが直接レンダリングをするわけではないので、ここで設定する意味は無いかも知れない。
					spriteRenderer->SetSprite(m_bitmapFont->GetSprite());

					// TODO:インスタンスを使いまわしているので設定値のリセットが必要かも？
					spriteRenderer->SetVisible(true);
				}
			}
		}

		// 一旦設定を親のものに統一
		SetScale(m_scale.x, m_scale.y);
		SetColor(m_color);
		SetAngle(m_angle);
		SetFlipX(m_flipX);
		SetFlipY(m_flipY);
		SetLayer(m_layer);
		SetOrderInLayer(m_orderInLayer);
		SetBlendMode(m_blend);
		SetLineSpacing(m_lineSpacing);
		SetVisibleSize(m_visibleSize);

		MeasurementBoxSize();

		return *this;
	}

	std::shared_ptr<SpriteRenderer> TextRenderer::GetLetter(size_t letterNum)
	{
		if (letterNum < m_vSpriteRenderer.size())
		{
			return m_vSpriteRenderer[letterNum];
		}

		return nullptr;
	}

	size_t TextRenderer::GetLetterSize() const
	{
		return m_text.size();
	}

	Size TextRenderer::GetBoxSize() const
	{
		return m_boxSize;
	}

	TextRenderer& TextRenderer::SetPos(const Point& pos)
	{
		m_pos = pos;
		return *this;
	}

	TextRenderer& TextRenderer::SetScale(float x, float y)
	{
		SetScaleX(x);
		SetScaleY(y);
		return *this;
	}

	TextRenderer& TextRenderer::SetScaleX(float x)
	{
		m_scale.x = x;

		for (auto& sprite : m_vSpriteRenderer)
		{
			sprite->SetScaleX(x);
		}

		return *this;
	}

	TextRenderer& TextRenderer::SetScaleY(float y)
	{
		m_scale.y = y;

		for (auto& sprite : m_vSpriteRenderer)
		{
			sprite->SetScaleY(y);
		}

		return *this;
	}

	TextRenderer& TextRenderer::SetColor(const Color& color)
	{
		m_color = color;

		for (auto& sprite : m_vSpriteRenderer)
		{
			sprite->SetColor(color);
		}

		return *this;
	}

	TextRenderer& TextRenderer::SetAngle(float angle)
	{
		m_angle = fmod(angle, 360.f);
		m_angleRad = m_angle * 3.14159265358979323846f / 180.f;	// ラジアンを算出

		for (auto& sprite : m_vSpriteRenderer)
		{
			sprite->SetAngle(angle);
		}

		return *this;
	}

	TextRenderer& TextRenderer::SetFlipX(bool isFlip)
	{
		m_flipX = isFlip;

		for (auto& sprite : m_vSpriteRenderer)
		{
			sprite->SetFlipX(m_flipX);
		}

		return *this;
	}

	TextRenderer& TextRenderer::SetFlipY(bool isFlip)
	{
		m_flipY = isFlip;

		for (auto& sprite : m_vSpriteRenderer)
		{
			sprite->SetFlipY(m_flipY);
		}

		return *this;
	}

	TextRenderer& TextRenderer::SetLayer(int layer)
	{
		m_layer = layer;

		for (auto& sprite : m_vSpriteRenderer)
		{
			sprite->SetLayer(layer);
		}
		return *this;
	}

	TextRenderer& TextRenderer::SetOrderInLayer(int32_t orderInLayer)
	{
		m_orderInLayer = orderInLayer;

		for (auto& sprite : m_vSpriteRenderer)
		{
			sprite->SetOrderInLayer(orderInLayer);
		}
		return *this;
	}

	TextRenderer& TextRenderer::SetBlendMode(BlendMode blend)
	{
		m_blend = blend;

		for (auto& sprite : m_vSpriteRenderer)
		{
			sprite->SetBlendMode(blend);
		}

		return *this;
	}

	TextRenderer& TextRenderer::SetTextHAlignment(TextHAlignment textHAlignment)
	{
		m_textHAlignment = textHAlignment;
		return *this;
	}

	TextRenderer& TextRenderer::SetPivot(PointF pivot)
	{
		m_pivot = { std::clamp( pivot.x, 0.f, 1.f), std::clamp(pivot.y, 0.f, 1.f) };
		return *this;
	}

	void TextRenderer::MeasurementBoxSize()
	{
		m_vCodeMap.resize(m_text.size());
		m_vWidth.clear();
		m_boxSize = {};

		// 左寄せ等倍と仮定してマップを作る
		if (m_bitmapFont)
		{
			Point pos;
			int count = 0;

			// テキストを1文字ずつ処理する
			for (auto& code : m_text)
			{
				stCodeMap codeMap;

				if (code == '\r')
				{
					// CRは無視する
				}
				else if (code == '\n')
				{
					codeMap.x = -'\n';
					m_vWidth.push_back(pos.x);	// この行の幅を保存

					// ボックスサイズ更新
					m_boxSize.x = std::max(m_boxSize.x, pos.x);	// 最大幅が更新されていたらそれをセット
					m_boxSize.y += m_height + m_lineSpacing;

					// 改行を行う
					pos.x = 0;
					pos.y += m_height + m_lineSpacing;
				}
				else
				{
					int atlasNum = m_bitmapFont->CodePointToAtlas(code);
					if (atlasNum >= 0)
					{
						const stSpriteAnimAtlas* atlas = m_bitmapFont->GetSprite()->GetAtlas(atlasNum);

						codeMap.x = pos.x + atlas->m_srcSize.x / 2;		// 座標を保存
						codeMap.atlas = atlasNum;
						pos.x += atlas->m_srcSize.x;
					}
					else
					{
						// スペースのグリフが存在しない場合、とりあえず高さと同じサイズの幅があると見なす
						if (code == ' ' || code == '\t')
						{
							pos.x += m_height;
						}
					}
				}

				m_vCodeMap[count] = codeMap;
				count++;
			}

			m_vWidth.push_back(pos.x);	// この行の幅を保存

			// ボックスサイズ更新
			m_boxSize.x = std::max(m_boxSize.x, pos.x);	// 最大幅が更新されていたらそれをセット
			m_boxSize.y += m_height;
		}
	}

	const std::u32string & TextRenderer::GetTextU32() const
	{
		return m_text;
	}

	TextRenderer& TextRenderer::SetLineSpacing(int32_t lineSpacing)
	{
		m_lineSpacing = lineSpacing;
		MeasurementBoxSize();

		return *this;
	}

	TextRenderer& Equisetum2::TextRenderer::SetVisibleSize(int32_t size)
	{
		m_visibleSize = size;
		return *this;
	}
	
	bool TextRenderer::Calculation()
	{
		Size sizeMax;
		int codeCount = 0;
		int rowCount = 0;
		int colCount = 0;
		int validLetterCount = 0;
		Point pos;	// box内の位置(次に文字を配置する座標)
		Size scaledBoxSize = { static_cast<int32_t>(m_boxSize.x * m_scale.x), static_cast<int32_t>(m_boxSize.y * m_scale.y) };

		// コードマップを1文字ずつ処理
		// TODO:回転処理
		for (auto& codeMap : m_vCodeMap)
		{
			auto& spriteRenderer = m_vSpriteRenderer[codeCount];

			if (colCount == 0)
			{
				switch (m_textHAlignment)
				{
				case TextHAlignment::Left:
				default:
					pos.x = 0;
					break;

				case TextHAlignment::Center:
					pos.x = (scaledBoxSize.x - static_cast<int32_t>(m_vWidth[rowCount] * m_scale.x)) / 2;
					break;

				case TextHAlignment::Right:
					pos.x = scaledBoxSize.x - static_cast<int32_t>(m_vWidth[rowCount] * m_scale.x);
					break;
				}
			}

			if (codeMap.x == -'\n')
			{
				// 改行を行う
				colCount = -1;
				rowCount++;

				pos.y = static_cast<int32_t>((m_height + m_lineSpacing) * rowCount * m_scale.y);
			}
			else if (codeMap.atlas >= 0)
			{
				spriteRenderer->SetAtlasNum(codeMap.atlas);

				Point spritePos;	// box内の相対座標

				spritePos.x = pos.x + static_cast<int32_t>(codeMap.x * m_scale.x);
				spriteRenderer->SetFlipX(m_flipX);
				if (m_flipX)
				{
					spritePos.x = scaledBoxSize.x - spritePos.x;
				}
				spriteRenderer->SetScaleX(m_scale.x);

				spritePos.y = pos.y + static_cast<int32_t>(m_height * m_scale.y) / 2;
				spriteRenderer->SetFlipY(m_flipY);
				if (m_flipY)
				{
					spritePos.y = scaledBoxSize.y - spritePos.y;
				}
				spriteRenderer->SetScaleY(m_scale.y);

				// pivotのオフセット算出
				auto offsetX = spritePos.x - static_cast<int32_t>(scaledBoxSize.x * m_pivot.x);
				auto offsetY = spritePos.y - static_cast<int32_t>(scaledBoxSize.y * m_pivot.y);

				// 回転なし？
				if (m_angle == 0)
				{
					spriteRenderer->SetPos(m_pos + Point{ offsetX, offsetY });
				}
				else
				{
					const auto cosVal = cos(m_angleRad);
					const auto sinVal = -sin(m_angleRad);
					Point spriteAnglePos;

					spriteAnglePos.x = static_cast<int32_t>(offsetX * cosVal - offsetY * sinVal);
					spriteAnglePos.y = static_cast<int32_t>(offsetX * sinVal + offsetY * cosVal);

					spriteRenderer->SetPos(m_pos + spriteAnglePos);
				}

				validLetterCount++;
			}

			spriteRenderer->Calculation();

			codeCount++;
			colCount++;
		}

		auto& vert = m_pImpl->m_vertex;
		auto& idx = m_pImpl->m_index;

		// 頂点とインデックスの数を設定
		vert.resize(validLetterCount * 4);
		idx.resize(validLetterCount * 6);

		// 頂点とインデックスを作成する(スプライトのバッチング)
		int vertexCount = 0;
		int indexCount = 0;
		codeCount = 0;
		for (auto& codeMap : m_vCodeMap)
		{
			// 指定されている文字数表示したらループを抜ける
			if (codeCount == m_visibleSize)
			{
				break;
			}

			if (codeMap.atlas >= 0)
			{
				auto& spriteRenderer = m_vSpriteRenderer[codeCount];

				if (spriteRenderer->IsVisible())
				{
					// 頂点
					auto vertexMax = spriteRenderer->m_pImpl->GetVertexCount();
					memcpy(&vert[vertexCount], spriteRenderer->m_pImpl->GetVertex(), vertexMax * sizeof(stVertexSprite));

					// インデックス
					auto pIndex = spriteRenderer->m_pImpl->GetIndex();
					auto indexMax = spriteRenderer->m_pImpl->GetIndexCount();
					for (size_t i = 0; i < indexMax; i++)
					{
						idx[indexCount + i] = vertexCount + pIndex[i];
					}

					vertexCount += vertexMax;
					indexCount += indexMax;
				}
			}

			codeCount++;
		}

		m_pImpl->m_vertexSize = vertexCount;
		m_pImpl->m_indexSize = indexCount;

		return true;
	}


	void LineRenderer::InitTest()
	{
		m_pImpl = std::make_shared<LineRenderer::Impl>();
	}

	std::shared_ptr<LineRenderer> LineRenderer::Create()
	{
		EQ_DURING
		{
			// インスタンス作成
			auto inst = std::make_shared<LineRenderer>();
			if (!inst)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			// インスタンス初期化
			inst->m_pImpl = std::make_shared<LineRenderer::Impl>();
			if (!inst->m_pImpl)
			{
				EQ_THROW(u8"インスタンスの初期化に失敗しました。");
			}

			// 予めある程度確保しておく
			const int32_t defaultSize = 32;
			inst->m_pImpl->m_vertex.resize(defaultSize);

			inst->m_vPos.resize(16);
			return inst;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return nullptr;
	}

	LineRenderer& LineRenderer::Clear()
	{
		m_vPosSize = 0;
		return *this;
	}

	LineRenderer& LineRenderer::PushLine(const Point& m_beginPos, const Point& m_endPos)
	{
		// サイズが足りない場合は拡張する
		if (static_cast<size_t>(m_vPosSize + 2) >= m_vPos.size())
		{
			m_vPos.resize(m_vPos.size() * 2);
		}

		// 始点と終点を追加する
		m_vPos[m_vPosSize] = m_beginPos;
		m_vPos[m_vPosSize+1] = m_endPos;
		m_vPosSize += 2;

		return *this;
	}

	LineRenderer& LineRenderer::SetColor(const Color& color)
	{
		m_color = color;
		return *this;
	}

	LineRenderer& LineRenderer::SetLayer(int layer)
	{
		m_layer = layer;
		return *this;
	}

	LineRenderer& LineRenderer::SetOrderInLayer(int32_t orderInLayer)
	{
		m_orderInLayer = orderInLayer;
		return *this;
	}

	LineRenderer& LineRenderer::SetBlendMode(BlendMode blend)
	{
		m_blend = blend;
		return *this;
	}

	bool LineRenderer::Calculation()
	{
		auto& vert = m_pImpl->m_vertex;
		auto& vertSize = m_pImpl->m_vertexSize;
		auto& blend = m_pImpl->m_blend;

		// サイズをクリア
		vertSize = 0;

		// ブレンドモードを設定する
		blend = m_blend;

		// サイズが足りない場合は拡張する
		if (static_cast<size_t>(m_vPosSize) > vert.size())
		{
			vert.resize(m_vPosSize);
		}

		// 頂点を作成する
		const uint32_t color = m_color.pixel;
		for (int32_t i = 0; i < m_vPosSize; i += 2)
		{
			// 始点
			{
				auto& curentVert = vert[vertSize];

				// 頂点
				curentVert.vertices[0] = static_cast<float>(m_vPos[i].x);
				curentVert.vertices[1] = static_cast<float>(m_vPos[i].y);
				auto dstColor = reinterpret_cast<uint32_t*>(curentVert.colors);
				*dstColor = color;
			}

			// 終点
			{
				auto& curentVert = vert[vertSize + 1];

				// 頂点
				curentVert.vertices[0] = static_cast<float>(m_vPos[i + 1].x);
				curentVert.vertices[1] = static_cast<float>(m_vPos[i + 1].y);
				auto dstColor = reinterpret_cast<uint32_t*>(curentVert.colors);
				*dstColor = color;
			}

			vertSize += 2;
		}

		return true;
	}



	void CircleRenderer::InitTest()
	{
		m_pImpl = std::make_shared<CircleRenderer::Impl>();
	}

	std::shared_ptr<CircleRenderer> CircleRenderer::Create()
	{
		EQ_DURING
		{
			// インスタンス作成
			auto inst = std::make_shared<CircleRenderer>();
			if (!inst)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			// インスタンス初期化
			inst->m_pImpl = std::make_shared<CircleRenderer::Impl>();
			if (!inst->m_pImpl)
			{
				EQ_THROW(u8"インスタンスの初期化に失敗しました。");
			}

			// 予めある程度確保しておく
			const int32_t defaultSize = 64;
			inst->m_pImpl->m_vertex.resize(defaultSize);

			return inst;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return nullptr;
	}

	CircleRenderer& CircleRenderer::SetCircle(const Point& centerPos, int32_t radius, bool solid)
	{
		m_pos = centerPos;
		m_radius = radius;
		m_solid = solid;
		return *this;
	}

	CircleRenderer& CircleRenderer::SetColor(const Color& color)
	{
		m_color = color;
		return *this;
	}

	CircleRenderer& CircleRenderer::SetLayer(int layer)
	{
		m_layer = layer;
		return *this;
	}

	CircleRenderer& CircleRenderer::SetOrderInLayer(int32_t orderInLayer)
	{
		m_orderInLayer = orderInLayer;
		return *this;
	}

	CircleRenderer& CircleRenderer::SetBlendMode(BlendMode blend)
	{
		m_blend = blend;
		return *this;
	}

	bool CircleRenderer::Calculation()
	{
		auto& vert = m_pImpl->m_vertex;
		auto& vertSize = m_pImpl->m_vertexSize;
		auto& blend = m_pImpl->m_blend;
		auto& solid = m_pImpl->m_solid;
		const int32_t segments = 32;
		const float coef = 2.0f * 3.14159265358979323846f / segments;

		// サイズをクリア
		vertSize = 0;

		// 描画モードを設定する
		blend = m_blend;
		solid = m_solid;

		// サイズが足りない場合は拡張する
		if (segments + 1 > vert.size())
		{
			vert.resize(segments + 1);
		}

		// 頂点を作成する
		const uint32_t color = m_color.pixel;
		for (int32_t i = 0; i <= segments; i++)
		{
			float rads = i * coef;
	
			vert[vertSize].vertices[0] = m_radius * cosf(rads) + m_pos.x;
			vert[vertSize].vertices[1] = m_radius * sinf(rads) + m_pos.y;
	
			auto vertexColor = reinterpret_cast<uint32_t*>(vert[vertSize].colors);
			*vertexColor = color;

			vertSize++;
		}

		return true;
	}


	void RectRenderer::InitTest()
	{
		m_pImpl = std::make_shared<RectRenderer::Impl>();

		// 予めある程度確保しておく
		m_pImpl->m_vertex.resize(vertexMax);
	}

	std::shared_ptr<RectRenderer> RectRenderer::Create()
	{
		EQ_DURING
		{
			// インスタンス作成
			auto inst = std::make_shared<RectRenderer>();
			if (!inst)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			// インスタンス初期化
			inst->m_pImpl = std::make_shared<RectRenderer::Impl>();
			if (!inst->m_pImpl)
			{
				EQ_THROW(u8"インスタンスの初期化に失敗しました。");
			}

			// 予めある程度確保しておく
			inst->m_pImpl->m_vertex.resize(vertexMax);

			return inst;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return nullptr;
	}

	RectRenderer& RectRenderer::SetRect(const Rect& rect, bool solid)
	{
		m_rect = rect;
		m_solid = solid;
		return *this;
	}

	RectRenderer& RectRenderer::SetColor(const Color& color)
	{
		for (auto& outColor : m_colors)
		{
			outColor = color;
		}
		return *this;
	}

	RectRenderer& RectRenderer::SetColor(const Color& leftTop, const Color& rightTop, const Color& leftBottom, const Color& rightBottom)
	{
		m_colors[0] = leftTop;
		m_colors[2] = rightTop;
		m_colors[1] = leftBottom;
		m_colors[3] = rightBottom;
		return *this;
	}

	RectRenderer& RectRenderer::SetLayer(int layer)
	{
		m_layer = layer;
		return *this;
	}

	RectRenderer& RectRenderer::SetOrderInLayer(int32_t orderInLayer)
	{
		m_orderInLayer = orderInLayer;
		return *this;
	}

	RectRenderer& RectRenderer::SetBlendMode(BlendMode blend)
	{
		m_blend = blend;
		return *this;
	}

	bool RectRenderer::Calculation()
	{
		auto& vert = m_pImpl->m_vertex;
		auto& vertSize = m_pImpl->m_vertexSize;
		auto& blend = m_pImpl->m_blend;
		auto& solid = m_pImpl->m_solid;

		// サイズをクリア
		vertSize = vertexMax;

		// 描画モードを設定する
		blend = m_blend;
		solid = m_solid;

		// 頂点
		{
			const int32_t cur = 0;

			vert[cur].vertices[0] = static_cast<GLfloat>(m_rect.x);
			vert[cur].vertices[1] = static_cast<GLfloat>(m_rect.y);

			auto vertexColor = reinterpret_cast<uint32_t*>(vert[cur].colors);
			*vertexColor = m_colors[cur].pixel;
		}
		{
			const int32_t cur = 1;

			vert[cur].vertices[0] = static_cast<GLfloat>(m_rect.x);
			vert[cur].vertices[1] = static_cast<GLfloat>(m_rect.y + m_rect.height);

			auto vertexColor = reinterpret_cast<uint32_t*>(vert[cur].colors);
			*vertexColor = m_colors[cur].pixel;
		}
		{
			const int32_t cur = solid ? 2 : 3;

			vert[cur].vertices[0] = static_cast<GLfloat>(m_rect.x + m_rect.width);
			vert[cur].vertices[1] = static_cast<GLfloat>(m_rect.y);

			auto vertexColor = reinterpret_cast<uint32_t*>(vert[cur].colors);
			*vertexColor = m_colors[solid ? cur : cur - 1].pixel;
		}
		{
			const int32_t cur = solid ? 3 : 2;

			vert[cur].vertices[0] = static_cast<GLfloat>(m_rect.x + m_rect.width);
			vert[cur].vertices[1] = static_cast<GLfloat>(m_rect.y + m_rect.height);

			auto vertexColor = reinterpret_cast<uint32_t*>(vert[cur].colors);
			*vertexColor = m_colors[solid ? cur : cur + 1].pixel;
		}

		return true;
	}
}

