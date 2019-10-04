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



