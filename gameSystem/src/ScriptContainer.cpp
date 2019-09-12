
#include "ScriptContainer.hpp"
#include "ScriptMapper.hpp"

EqHeap::Container<ScriptContainer> ScriptContainer::Create(const String& scriptName)
{
	auto heap = Singleton<EqHeap>::GetInstance();
	auto scriptMapper = Singleton<ScriptMapper>::GetInstance();

	// �X�N���v�g�R���e�i�̃C���X�^���X���쐬
	auto inst = heap->New<ScriptContainer>();
	if (auto ref = inst.Ref())
	{
		// �X�N���v�g���̃C���f�b�N�X���擾
		ref->m_index = scriptMapper->ToIndex(scriptName);
		if (ref->m_index >= 0)
		{
			// �X�N���v�g�����擾
			if (const ScriptMapper::Value* p = scriptMapper->Get(ref->m_index))
			{
				// �X�N���v�g�̃C���X�^���X���쐬
				ref->m_ctx = heap->New(p->instanceSize);

				// �X�N���v�g�̃R���X�g���N�^���Ăяo��
				if (p->constructor(ref->m_ctx))
				{
					heap->AddRef(ref->m_ctx);
					return inst;
				}
			}
		}
	}

	return  EqHeap::Container<ScriptContainer>{};
}

ScriptContainer::~ScriptContainer()
{
	auto heap = Singleton<EqHeap>::GetInstance();
	auto scriptMapper = Singleton<ScriptMapper>::GetInstance();

	if (heap->DecRef(m_ctx) == 0)
	{
		// �X�N���v�g�����擾
		if (const ScriptMapper::Value* p = scriptMapper->Get(m_index))
		{
			// �X�N���v�g�̃f�X�g���N�^���Ăяo��
			p->destructor(m_ctx);
			// �X�N���v�g�̃C���X�^���X���폜
			heap->Delete(m_ctx);
		}
	}
}

bool ScriptContainer::OnCreate(Object* owner)
{
	auto scriptMapper = Singleton<ScriptMapper>::GetInstance();

	// �X�N���v�g�����擾
	if (const ScriptMapper::Value* p = scriptMapper->Get(m_index))
	{
		if (p->onCreate(m_ctx, owner))
		{
			m_isCreated = true;
			return true;
		}
	}
	return false;
}

bool ScriptContainer::OnStart(Object * owner)
{
	auto scriptMapper = Singleton<ScriptMapper>::GetInstance();

	// ���������쐬�������Ă���H
	if (m_isCreated)
	{
		// �X�N���v�g�����擾
		if (const ScriptMapper::Value* p = scriptMapper->Get(m_index))
		{
			if(p->onStart(m_ctx, owner))
			{
				m_isStarted = true;
				return true;
			}
		}
	}

	return false;
}

bool ScriptContainer::FixedUpdate(Object* owner)
{
	auto scriptMapper = Singleton<ScriptMapper>::GetInstance();

	if (!m_isStarted)
	{
		if (!OnStart(owner))
		{
			return false;
		}
	}

	// �X�N���v�g�����擾
	if (const ScriptMapper::Value* p = scriptMapper->Get(m_index))
	{
		return p->fixedUpdate(m_ctx, owner);
	}
	return false;
}


