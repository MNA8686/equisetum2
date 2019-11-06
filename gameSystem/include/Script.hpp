#if !defined(_EQSCRIPT_H_)
#define _EQSCRIPT_H_

#include "Equisetum2.h"
using namespace Equisetum2;

// �X�N���v�g����͂��̃t�@�C���̂݃C���N���[�h����΂����悤�ɂ��Ă���
#include "ResourceMapper.hpp"
#include "ScriptMapper.hpp"
#include "Object.hpp"
#include "RendererContainer.hpp"
#include "AudioContainer.hpp"

class ScriptBase
{
public:
	static std::shared_ptr<Renderer>& GetRenderer(void);
	static std::shared_ptr<Renderer> m_renderer;		// �b��

	/// �X�N���v�g�C���X�^���X�������ɌĂяo�����
	bool OnCreate(Object* owner)
	{
		return true;
	}
	
	/// �ŏ���FixedUpdate()�Ăяo���O�ɌĂяo�����
	bool OnStart(Object* owner)
	{
		return true;
	}
	
	/// FixedUpdate()�Ăяo���O�ɌĂяo�����
	bool BeforeUpdate(Object* owner)
	{
		return true;
	}
	
	/// ���t���[���Ăяo����鏈��
	bool FixedUpdate(Object* owner)
	{
		return true;
	}
	
	/// FixedUpdate()�̂��ƂɌĂяo�����
	bool LateUpdate(Object* owner)
	{
		return true;
	}
};

#endif
