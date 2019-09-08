#if !defined(_EQSCRIPT_H_)
#define _EQSCRIPT_H_

#include "Equisetum2.h"
using namespace Equisetum2;

#include "ResourceMapper.hpp"
#include "ScriptMapper.hpp"
#include "Object.hpp"
#include "RendererContainer.hpp"

class ScriptBase
{
public:
	static std::shared_ptr<Renderer>& GetRenderer(void);
	static std::shared_ptr<Renderer> m_renderer;		// �b��

	bool OnCreate(Object* owner)
	{
		return true;
	}
	bool FixedUpdate(Object* owner)
	{
		return true;
	}
};

#if 0
// �X�N���v�g�e�[�u���^��`
class Script;
using funcCreateScript = std::function<std::shared_ptr<Script>()>;
typedef struct
{
	String id;				// �X�N���v�gID
	funcCreateScript func;	// �X�N���v�g�I�u�W�F�N�g�擾���\�b�h
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

	//void SetOwner(std::shared_ptr<Object>& ownerObject);
	void SetOwner(NodeHandler ownerObject);
	bool Start();

	static void SetScriptTbl(const std::vector<stScriptTbl>& tbl);
	static std::shared_ptr<Script> Create(const String& id);
	static std::shared_ptr<Renderer>& GetRenderer(void);
	static std::shared_ptr<Renderer> m_renderer;		// �b��

	/**
	* @brief �C���[�W��ID��ݒ肷��
	*/
	void SetIdentify(const String& id);

	/**
	* @brief �C���[�W��ID���擾����
	* @return �C���[�W��ID
	*/
	String Identify() const;

private:
	bool m_isStarted = false;					/// ���̃X�N���v�g���J�n�ς݂��ǂ���
	String m_identify;
	NodeHandler m_ownerObject;		/// ���̃X�N���v�g���������Ă���I�u�W�F�N�g

protected:
	NodeHandler GetOwner();
};
#endif

#endif
