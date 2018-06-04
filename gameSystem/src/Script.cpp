
#include "Script.hpp"

#define EQ_SCRIPT_DEFINE(name) { #name, []()->std::shared_ptr<Script> { return std::make_shared<name>(); } }

using funcCreateScript = std::function<std::shared_ptr<Script>()>;
typedef struct
{
	String name;
	funcCreateScript func;
}stScriptTbl;

#if 0
static const stScriptTbl g_scriptTbl[] =
{
	EQ_SCRIPT_DEFINE(ScriptTest1),
	EQ_SCRIPT_DEFINE(ScriptTest2),
	EQ_SCRIPT_DEFINE(ScriptMain),
};
#endif

std::shared_ptr<Script> Script::Create(const String& name)
{
#if 0
	for (auto& e : g_scriptTbl)
	{
		if (e.name == name)
		{
			return e.func();
		}
	}
#endif
	return nullptr;
}

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

bool Script::Render()
{
	return true;
}
