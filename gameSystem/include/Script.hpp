#if !defined(_EQSCRIPT_H_)
#define _EQSCRIPT_H_

#include "Equisetum2.h"

#include "Object.hpp"
#include <cereal/cereal.hpp>

using namespace Equisetum2;

// スクリプトテーブル型定義
class Script;
using funcCreateScript = std::function<std::shared_ptr<Script>()>;
typedef struct
{
	String id;				// スクリプトID
	funcCreateScript func;	// スクリプトオブジェクト取得メソッド
}stScriptTbl;
#define EQ_SCRIPT_DEFINE(id) { String(#id), []()->std::shared_ptr<Script> { return std::make_shared<id>(); } }

class Object;
class Script
{
public:
	Script();
	virtual ~Script();

	virtual bool OnCreate();
	virtual bool OnStart();
	virtual bool FixedUpdate();

	void SetOwner(std::shared_ptr<Object>& ownerObject);
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


	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(CEREAL_NVP(m_isStarted));
		archive(CEREAL_NVP(m_ownerObject));

		std::string& str = m_identify;
		archive(CEREAL_NVP(str));
	}

private:
	bool m_isStarted = false;					/// このスクリプトが開始済みかどうか
	String m_identify;
	std::weak_ptr<Object> m_ownerObject;		/// このスクリプトを所持しているオブジェクト

protected:
	std::shared_ptr<Object> GetOwner();
};


#endif
