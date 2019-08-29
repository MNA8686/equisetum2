
#include "Script.hpp"

std::shared_ptr<Renderer> ScriptBase::m_renderer;

std::shared_ptr<Renderer>& ScriptBase::GetRenderer(void)
{
	return m_renderer;
}

#if 0

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

//void Script::SetOwner(std::shared_ptr<Object>& ownerObject)
void Script::SetOwner(NodeHandler ownerObject)
{
	m_ownerObject = ownerObject;
}

bool Script::Start()
{
	bool ret = true;

	// スクリプトが開始してない？
	if (!m_isStarted)
	{
		// スクリプトを開始する
		ret = OnStart();
		m_isStarted = true;
	}

	return ret;
}

void Script::SetIdentify(const String& id)
{
	m_identify = id;
}

String Script::Identify() const
{
	return m_identify;
}

//std::shared_ptr<Object> Script::GetOwner()
NodeHandler Script::GetOwner()
{
	//return m_ownerObject.lock();
	return m_ownerObject;
}
#endif

