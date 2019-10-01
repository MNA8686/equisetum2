#if !defined(_EQSCRIPTCONTAINER_H_)
#define _EQSCRIPTCONTAINER_H_

#include "Equisetum2.h"
using namespace Equisetum2;

#include "EqHeap.hpp"

class Object;
class ScriptContainer
{
public:
	static EqHeap::Container<ScriptContainer> Create(Object* owner, const String& scriptName);
	static Object* GetCurrentObject();

	ScriptContainer() = default;
	~ScriptContainer();

	template <class T>
	T* Ref()
	{
		auto heap = Singleton<EqHeap>::GetInstance();
		auto scriptMapper = Singleton<ScriptMapper>::GetInstance();

		if (const ScriptMapper::Value* value = scriptMapper->Get(m_index))
		{
			if (auto ref = heap->Ref<T>(m_ctx))
			{
				// 型が合っているかチェック
				//if (EQ_SCRIPT_REGISTER_NAME(T)::Hash() == value->hash)
				//if ((T##_register)::Hash() == value->hash)
				{
					return ref;
				}
			}
		}

		return nullptr;
	}

	bool OnCreate(Object* owner);
	bool FixedUpdate(Object* owner);

private:
	bool OnStart(Object* owner);

	EqHeap::Handler m_ctx = 0;	// スクリプトの実体
	int32_t m_index = -1;		// スクリプトマッパーのインデックス
	bool m_isCreated = false;
	bool m_isStarted = false;
};
#endif
