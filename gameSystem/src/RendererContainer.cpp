#include "RendererContainer.hpp"

SpriteRendererContainer::SpriteRendererContainer()
{
	if (auto obj = Object::GetCreatingObject())
	{
		// スプライトレンダラを作る
		if (auto newRenderer = ScriptBase::GetRenderer()->CreateRenderObject<SpriteRenderer>())
		{
			int32_t rendererIndex = obj->AddRenderObject(newRenderer);
			if (rendererIndex >= 0)
			{
				m_nodeHandler = obj->GetNodeHandler();
				m_rendererIndex = rendererIndex;
			}
		}
	}
}

SpriteRenderer* SpriteRendererContainer::Ref()
{
	if (auto obj = Object::GetObjectByHandler(m_nodeHandler))
	{
		if (auto renderObject = obj->GetRenderObject(m_rendererIndex))
		{
			if (renderObject->GetType() == RenderType::SPRITE)
			{
				return static_cast<SpriteRenderer*>(renderObject);
			}
		}
	}

	return nullptr;
}

SpriteRenderer* SpriteRendererContainer::operator->()
{
	return Ref();
}

//-----------------------------------------------------------------------------------------------

AnimationContainer::AnimationContainer()
{
	if (auto obj = Object::GetCreatingObject())
	{
		m_nodeHandler = obj->GetNodeHandler();
		m_sprite->SetVisible(false);
	}
}

bool AnimationContainer::SetAnimation(int32_t assetAnimation)
{
	m_assetAnimation = assetAnimation;
	m_count = 0;

	Update();

	return true;
}

void AnimationContainer::SetDegree(int32_t degree)
{
	m_degree = degree;
}

int32_t AnimationContainer::TagToInt(const String& tag) const
{
	if (auto obj = Object::GetObjectByHandler(m_nodeHandler))
	{
		auto& anim = obj->GetAsset()->m_animation[m_assetAnimation];
		return anim->TagToInt(tag);
	}

	return -1;
}

void AnimationContainer::Start(int32_t tagIndex)
{
	m_tagIndex = tagIndex;
	m_count = 0;

	Update();
}

bool AnimationContainer::Inc(int32_t delta)
{
	m_count += delta;

	Update();

	return true;
}

SpriteRenderer* AnimationContainer::GetSpriteRenderer()
{
	return m_sprite.Ref();
}

void AnimationContainer::Update()
{
	//if (m_tagIndex < 0)
	//{
	//	return;
	//}

	if (Object* obj = Object::GetObjectByHandler(m_nodeHandler))
	{
		auto& anim = obj->GetAsset()->m_animation[m_assetAnimation];
		if (const std::shared_ptr<AnimationTimeline> timeline = anim->GetTimeline(m_tagIndex))
		{
			// 現在時刻に対応するアニメーション番号を取得する
			int32_t index = timeline->GetIndexByTime(m_count);
			// アニメーション番号からスプライトなどのデータを取り出す
			const stAnimationElement* elem = timeline->GetElement(index);

			// 回転をサポートしているならオフセットを取得する
			int32_t offset = timeline->GetRotateOffset(m_degree);

			auto sprite = m_sprite.Ref();
			if (sprite->GetSprite() != elem->m_sprite.get())
			{
				sprite->SetSprite(elem->m_sprite);
			}
			sprite->SetAtlasNum(elem->m_sprite->ToAtlasNumWithTagIndex(elem->m_tagIndex, elem->m_ptr) + offset);
		}
	}
}

//-----------------------------------------------------------------------------------------------

LineRendererContainer::LineRendererContainer()
{
	if (auto obj = Object::GetCreatingObject())
	{
		// ラインレンダラを作る
		if (auto newRenderer = ScriptBase::GetRenderer()->CreateRenderObject<LineRenderer>())
		{
			int32_t rendererIndex = obj->AddRenderObject(newRenderer);
			if (rendererIndex >= 0)
			{
				m_nodeHandler = obj->GetNodeHandler();
				m_rendererIndex = rendererIndex;
			}
		}
	}
}

LineRenderer* LineRendererContainer::Ref()
{
	if (auto obj = Object::GetObjectByHandler(m_nodeHandler))
	{
		if (auto renderObject = obj->GetRenderObject(m_rendererIndex))
		{
			if (renderObject->GetType() == RenderType::PRIMITIVE &&
				renderObject->GetSubType() == PrimitiveType::LINE)
			{
				return static_cast<LineRenderer*>(renderObject);
			}
		}
	}

	return nullptr;
}

LineRenderer* LineRendererContainer::operator->()
{
	return Ref();
}

//-----------------------------------------------------------------------------------------------

RectRendererContainer::RectRendererContainer()
{
	if (auto obj = Object::GetCreatingObject())
	{
		// レクトレンダラを作る
		if (auto newRenderer = ScriptBase::GetRenderer()->CreateRenderObject<RectRenderer>())
		{
			int32_t rendererIndex = obj->AddRenderObject(newRenderer);
			if (rendererIndex >= 0)
			{
				m_nodeHandler = obj->GetNodeHandler();
				m_rendererIndex = rendererIndex;
			}
		}
	}
}

RectRenderer* RectRendererContainer::Ref()
{
	if (auto obj = Object::GetObjectByHandler(m_nodeHandler))
	{
		if (auto renderObject = obj->GetRenderObject(m_rendererIndex))
		{
			if (renderObject->GetType() == RenderType::PRIMITIVE &&
				renderObject->GetSubType() == PrimitiveType::RECT)
			{
				return static_cast<RectRenderer*>(renderObject);
			}
		}
	}

	return nullptr;
}

RectRenderer* RectRendererContainer::operator->()
{
	return Ref();
}

//-----------------------------------------------------------------------------------------------

CircleRendererContainer::CircleRendererContainer()
{
	if (auto obj = Object::GetCreatingObject())
	{
		// サークルレンダラを作る
		if (auto newRenderer = ScriptBase::GetRenderer()->CreateRenderObject<CircleRenderer>())
		{
			int32_t rendererIndex = obj->AddRenderObject(newRenderer);
			if (rendererIndex >= 0)
			{
				m_nodeHandler = obj->GetNodeHandler();
				m_rendererIndex = rendererIndex;
			}
		}
	}
}

CircleRenderer* CircleRendererContainer::Ref()
{
	if (auto obj = Object::GetObjectByHandler(m_nodeHandler))
	{
		if (auto renderObject = obj->GetRenderObject(m_rendererIndex))
		{
			if (renderObject->GetType() == RenderType::PRIMITIVE &&
				renderObject->GetSubType() == PrimitiveType::CIRCLE)
			{
				return static_cast<CircleRenderer*>(renderObject);
			}
		}
	}

	return nullptr;
}

CircleRenderer* CircleRendererContainer::operator->()
{
	return Ref();
}

//-----------------------------------------------------------------------------------------------

TextRendererContainer::TextRendererContainer()
{
	if (auto obj = Object::GetCreatingObject())
	{
		// テキストレンダラを作る
		if (auto newRenderer = ScriptBase::GetRenderer()->CreateRenderObject<TextRenderer>())
		{
			int32_t rendererIndex = obj->AddRenderObject(newRenderer);
			if (rendererIndex >= 0)
			{
				m_nodeHandler = obj->GetNodeHandler();
				m_rendererIndex = rendererIndex;
			}
		}
	}
}

TextRenderer* TextRendererContainer::Ref()
{
	if (auto obj = Object::GetObjectByHandler(m_nodeHandler))
	{
		if (auto renderObject = obj->GetRenderObject(m_rendererIndex))
		{
			if (renderObject->GetType() == RenderType::TEXT)
			{
				return static_cast<TextRenderer*>(renderObject);
			}
		}
	}

	return nullptr;
}

TextRenderer* TextRendererContainer::operator->()
{
	return Ref();
}



