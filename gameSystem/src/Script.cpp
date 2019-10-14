
#include "Script.hpp"

std::shared_ptr<Renderer> ScriptBase::m_renderer;

std::shared_ptr<Renderer>& ScriptBase::GetRenderer(void)
{
	return m_renderer;
}


