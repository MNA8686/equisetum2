
#include "Script.hpp"


static std::vector<stScriptTbl> g_scriptTbl;

void Script::SetScriptTbl(const std::vector<stScriptTbl>& tbl)
{
	g_scriptTbl = tbl;
}

std::shared_ptr<Script> Script::Create(const String& id)
{
	for (auto& e : g_scriptTbl)
	{
		if (e.id == id)
		{
			return e.func();
		}
	}

	return nullptr;
}

std::shared_ptr<Renderer>& Script::GetRenderer(void)
{
	return m_renderer;
}

std::shared_ptr<Renderer> Script::m_renderer;

Script::Script()
{

}

Script::~Script()
{

}

bool Script::OnCreate()
{
	return true;
}

bool Script::OnStart()
{
	return true;
}

bool Script::FixedUpdate()
{
	return true;
}

void Script::SetOwner(std::shared_ptr<Object>& ownerObject)
{
	m_ownerObject = ownerObject;
}
