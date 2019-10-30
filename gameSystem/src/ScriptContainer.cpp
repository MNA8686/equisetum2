
#include "ScriptContainer.hpp"
#include "ScriptMapper.hpp"

static Object* pCurrentObject = nullptr;

EqHeap::Container<ScriptContainer> ScriptContainer::Create(Object* owner, const String& scriptName)
{
	auto heap = Singleton<EqHeap>::GetInstance();
	auto scriptMapper = Singleton<ScriptMapper>::GetInstance();
	EqHeap::Container<ScriptContainer> retInst;

	pCurrentObject = owner;

	// スクリプトコンテナのインスタンスを作成
	auto inst = heap->New<ScriptContainer>();
	if (auto ref = inst.Ref())
	{
		// スクリプト情報のインデックスを取得
		ref->m_index = scriptMapper->ToIndex(scriptName);
		if (ref->m_index >= 0)
		{
			// スクリプト情報を取得
			if (const ScriptMapper::Value* p = scriptMapper->Get(ref->m_index))
			{
				// スクリプトのインスタンスを作成
				ref->m_ctx = heap->New(p->instanceSize);

				// スクリプトのコンストラクタを呼び出す
				if (p->constructor(ref->m_ctx))
				{
					heap->AddRef(ref->m_ctx);
					retInst = inst;
				}
			}
		}
	}

	pCurrentObject = nullptr;

	return retInst;
}

Object* ScriptContainer::GetCurrentObject()
{
	return pCurrentObject;
}

ScriptContainer::~ScriptContainer()
{
	auto heap = Singleton<EqHeap>::GetInstance();
	auto scriptMapper = Singleton<ScriptMapper>::GetInstance();

	if (heap->DecRef(m_ctx) == 0)
	{
		// スクリプト情報を取得
		if (const ScriptMapper::Value* p = scriptMapper->Get(m_index))
		{
			// スクリプトのデストラクタを呼び出す
			p->destructor(m_ctx);
			// スクリプトのインスタンスを削除
			heap->Delete(m_ctx);
		}
	}
}

bool ScriptContainer::OnCreate(Object* owner)
{
	auto scriptMapper = Singleton<ScriptMapper>::GetInstance();

	// スクリプト情報を取得
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

	// そもそも作成成功している？
	if (m_isCreated)
	{
		// スクリプト情報を取得
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

bool ScriptContainer::BeforeUpdate(Object* owner)
{
	auto scriptMapper = Singleton<ScriptMapper>::GetInstance();

	// スクリプト情報を取得
	if (const ScriptMapper::Value* p = scriptMapper->Get(m_index))
	{
		return p->beforeUpdate(m_ctx, owner);
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

	// スクリプト情報を取得
	if (const ScriptMapper::Value* p = scriptMapper->Get(m_index))
	{
		return p->fixedUpdate(m_ctx, owner);
	}
	return false;
}

bool ScriptContainer::LateUpdate(Object* owner)
{
	auto scriptMapper = Singleton<ScriptMapper>::GetInstance();

	// スクリプト情報を取得
	if (const ScriptMapper::Value* p = scriptMapper->Get(m_index))
	{
		return p->lateUpdate(m_ctx, owner);
	}
	return false;
}


