#include "RendererContainer.hpp"

bool SpriteRendererContainer::BindObject(Object* obj)
{
	// ���Ƀo�C���h����Ă��Ȃ��H
	if (m_nodeHandler.id < 0 && obj)
	{
		// �X�v���C�g�����_�������
		if (auto newRenderer = ScriptBase::GetRenderer()->CreateRenderObject<SpriteRenderer>())
		{
			int32_t rendererIndex = obj->AddRenderObject(newRenderer);
			if (rendererIndex >= 0)
			{
				m_nodeHandler = obj->GetNodeHandler();
				m_rendererIndex = rendererIndex;
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


bool LineRendererContainer::BindObject(Object* obj)
{
	// ���Ƀo�C���h����Ă��Ȃ��H
	if (m_nodeHandler.id < 0 && obj)
	{
		// ���C�������_�������
		if (auto newRenderer = ScriptBase::GetRenderer()->CreateRenderObject<LineRenderer>())
		{
			int32_t rendererIndex = obj->AddRenderObject(newRenderer);
			if (rendererIndex >= 0)
			{
				m_nodeHandler = obj->GetNodeHandler();
				m_rendererIndex = rendererIndex;
				return true;
			}
		}
	}

	return false;
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



bool RectRendererContainer::BindObject(Object* obj)
{
	// ���Ƀo�C���h����Ă��Ȃ��H
	if (m_nodeHandler.id < 0 && obj)
	{
		// ���N�g�����_�������
		if (auto newRenderer = ScriptBase::GetRenderer()->CreateRenderObject<RectRenderer>())
		{
			int32_t rendererIndex = obj->AddRenderObject(newRenderer);
			if (rendererIndex >= 0)
			{
				m_nodeHandler = obj->GetNodeHandler();
				m_rendererIndex = rendererIndex;
				return true;
			}
		}
	}

	return false;
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



bool CircleRendererContainer::BindObject(Object* obj)
{
	// ���Ƀo�C���h����Ă��Ȃ��H
	if (m_nodeHandler.id < 0 && obj)
	{
		// �T�[�N�������_�������
		if (auto newRenderer = ScriptBase::GetRenderer()->CreateRenderObject<CircleRenderer>())
		{
			int32_t rendererIndex = obj->AddRenderObject(newRenderer);
			if (rendererIndex >= 0)
			{
				m_nodeHandler = obj->GetNodeHandler();
				m_rendererIndex = rendererIndex;
				return true;
			}
		}
	}

	return false;
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


