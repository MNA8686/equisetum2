#if !defined(_EQSCRIPT_H_)
#define _EQSCRIPT_H_

#include "Equisetum2.h"
using namespace Equisetum2;

#include "Object.hpp"
#include <cereal/cereal.hpp>

class Script
{
public:
	Script();
	virtual ~Script();

	virtual bool OnCreate();
	virtual bool OnStart();
	virtual bool FixedUpdate();
	virtual bool Render();

	static std::shared_ptr<Script> Create(const String& name);

	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(CEREAL_NVP(m_base));
	}

	int m_base = 0;
};

// ここにインクルード
//#include "script/ScriptTest1.hpp"
//#include "script/ScriptTest2.hpp"
//#include "script/ScriptMain.hpp"

#endif
