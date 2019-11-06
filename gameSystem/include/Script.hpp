#if !defined(_EQSCRIPT_H_)
#define _EQSCRIPT_H_

#include "Equisetum2.h"
using namespace Equisetum2;

// スクリプトからはこのファイルのみインクルードすればいいようにしておく
#include "ResourceMapper.hpp"
#include "ScriptMapper.hpp"
#include "Object.hpp"
#include "RendererContainer.hpp"
#include "AudioContainer.hpp"

class ScriptBase
{
public:
	static std::shared_ptr<Renderer>& GetRenderer(void);
	static std::shared_ptr<Renderer> m_renderer;		// 暫定

	/// スクリプトインスタンス生成時に呼び出される
	bool OnCreate(Object* owner)
	{
		return true;
	}
	
	/// 最初のFixedUpdate()呼び出し前に呼び出される
	bool OnStart(Object* owner)
	{
		return true;
	}
	
	/// FixedUpdate()呼び出し前に呼び出される
	bool BeforeUpdate(Object* owner)
	{
		return true;
	}
	
	/// 毎フレーム呼び出される処理
	bool FixedUpdate(Object* owner)
	{
		return true;
	}
	
	/// FixedUpdate()のあとに呼び出される
	bool LateUpdate(Object* owner)
	{
		return true;
	}
};

#endif
