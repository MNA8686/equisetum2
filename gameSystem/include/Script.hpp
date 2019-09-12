#if !defined(_EQSCRIPT_H_)
#define _EQSCRIPT_H_

#include "Equisetum2.h"
using namespace Equisetum2;

// スクリプトからはこのファイルのみインクルードすればいいようにしておく
#include "ResourceMapper.hpp"
#include "ScriptMapper.hpp"
#include "Object.hpp"
#include "RendererContainer.hpp"

class ScriptBase
{
public:
	static std::shared_ptr<Renderer>& GetRenderer(void);
	static std::shared_ptr<Renderer> m_renderer;		// 暫定

	bool OnCreate(Object* owner)
	{
		return true;
	}
	bool OnStart(Object* owner)
	{
		return true;
	}
	bool FixedUpdate(Object* owner)
	{
		return true;
	}
};

#if 0
class Object;
class Script
{
public:
	Script();
	virtual ~Script();

	virtual bool OnCreate();
	virtual bool OnStart();
	virtual bool FixedUpdate();

	//void SetOwner(std::shared_ptr<Object>& ownerObject);
	void SetOwner(NodeHandler ownerObject);
	bool Start();

	static void SetScriptTbl(const std::vector<stScriptTbl>& tbl);
	static std::shared_ptr<Script> Create(const String& id);
	static std::shared_ptr<Renderer>& GetRenderer(void);
	static std::shared_ptr<Renderer> m_renderer;		// 暫定

	/**
	* @brief イメージのIDを設定する
	*/
	void SetIdentify(const String& id);

	/**
	* @brief イメージのIDを取得する
	* @return イメージのID
	*/
	String Identify() const;

private:
	bool m_isStarted = false;					/// このスクリプトが開始済みかどうか
	String m_identify;
	NodeHandler m_ownerObject;		/// このスクリプトを所持しているオブジェクト

protected:
	NodeHandler GetOwner();
};
#endif

#endif
