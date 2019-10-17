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
	}

#if 0
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
	}r
#endif
}

bool AnimationContainer::SetAnimation(int32_t assetAnimation)
{
	m_assetAnimation = assetAnimation;
	m_count = 0;

	if (auto obj = Object::GetObjectByHandler(m_nodeHandler))
	{
		auto& anim = obj->GetAsset()->m_animation[m_assetAnimation];
		int32_t index = anim->GetIndexByTime(0, m_count);
		
		m_sprite->SetSprite(anim->GetElement(0, index)->m_sprite);
	}

	return true;
}

bool AnimationContainer::Inc(int32_t delta)
{
	m_count += delta;

	if (auto obj = Object::GetObjectByHandler(m_nodeHandler))
	{
		auto& anim = obj->GetAsset()->m_animation[m_assetAnimation];
		int32_t index = anim->GetIndexByTime(0, m_count);
		int32_t offset = anim->GetRotateOffset(0, 0);
		auto elem = anim->GetElement(0, index);
		
		m_sprite->SetSprite(elem->m_sprite).
			SetAtlasNum(elem->m_sprite->ToAtlasNumWithTagIndex(elem->m_tagIndex, elem->m_ptr) + offset).
			SetLayer(5).
			SetBlendMode(BlendMode::Blend).
			SetPos({ 448 / 2, 480 / 2 }).
			SetScale(1.5f, 1.5f);
	}

	return true;
}

#if 0
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
#endif

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



