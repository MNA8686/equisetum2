#include "RendererContainer.hpp"

bool SpriteRendererContainer::BindObject(Object* obj)
{
	// 既にバインドされていない？
	if (m_nodeHandler.id < 0 && obj)
	{
		// スプライトレンダラを作る
		if (auto newSpriteRenderer = ScriptBase::GetRenderer()->CreateRenderObject<SpriteRenderer>())
		{
			int32_t spriteRendererIndex = obj->AddRenderObject(newSpriteRenderer);
			if (spriteRendererIndex >= 0)
			{
				m_nodeHandler = obj->GetNodeHandler();
				m_spriteRendererIndex = spriteRendererIndex;
				return true;
			}
		}
	}

	return false;
}

SpriteRenderer* SpriteRendererContainer::Ref()
{
	if (auto obj = Object::GetObjectByHandler(m_nodeHandler))
	{
		if (auto renderObject = obj->GetRenderObject(m_spriteRendererIndex))
		{
			if (renderObject->GetType() == RenderType::SPRITE)
			{
				if (auto spriteRenderer = static_cast<SpriteRenderer*>(renderObject))
				{
					return spriteRenderer;
				}
			}
		}
	}

	return nullptr;
}

SpriteRenderer* SpriteRendererContainer::operator->()
{
	return Ref();
}

