#if !defined(_EQSCRIPTMAPPER_H_)
#define _EQSCRIPTMAPPER_H_

#include "Equisetum2.h"
using namespace Equisetum2;

#include "EqHeap.hpp"

class Object;
class ScriptMapper final
{
public:
	friend Singleton<ScriptMapper>;	// シングルトンからインスタンスを作成してもらえるようにする

	struct Value
	{
		String id;			/// スクリプト名
		size_t hash = 0;	/// ダイナミックキャスト用のハッシュ
		size_t instanceSize = 0;	/// スクリプトインスタンスのサイズ

		// シリアライズの都合でポリモーフィック技が使えないので、スクリプトは関数オブジェクトを介して呼ぶ
		std::function<bool (EqHeap::Handler handler)> constructor;
		std::function<bool (EqHeap::Handler handler)> destructor;
		std::function<bool (EqHeap::Handler handler, Object* owner)> onCreate;
		std::function<bool (EqHeap::Handler handler, Object* owner)> onStart;
		std::function<bool (EqHeap::Handler handler, Object* owner)> fixedUpdate;
	};

	void Register(const Value& value);
	int32_t ToIndex(const String& id) const;
	const Value* Get(int32_t index) const;

private:
	// スクリプトレジスタテーブル
	std::vector<Value> m_vValue;
};

#define EQ_SCRIPT_REGISTER_NAME(NAME) NAME##_register

#define EQ_SCRIPT_REGISTER(NAME) \
	class EQ_SCRIPT_REGISTER_NAME(NAME)\
	{\
	public:\
		EQ_SCRIPT_REGISTER_NAME(NAME)()\
		{\
			ScriptMapper::Value val;\
			size_t hash = std::hash<String>()(#NAME);\
			m_hash = hash;\
			val.id = #NAME;\
			val.hash = hash;\
			val.instanceSize = sizeof(NAME);\
			val.constructor = [](EqHeap::Handler handler)->bool {\
				auto heap = Singleton<EqHeap>::GetInstance();\
				if (auto p = heap->Ref<NAME>(handler))\
				{\
					new (p) NAME;\
					return true;\
				}\
				return false;\
			};\
			val.destructor = [](EqHeap::Handler handler)->bool {\
				auto heap = Singleton<EqHeap>::GetInstance();\
				if (auto p = heap->Ref<NAME>(handler))\
				{\
					p->~NAME();\
					return true;\
				}\
				return false;\
			};\
			val.onCreate = [](EqHeap::Handler handler, Object* owner)->bool {\
				auto heap = Singleton<EqHeap>::GetInstance();\
				if (auto p = heap->Ref<NAME>(handler))\
				{\
					return p->OnCreate(owner);\
				}\
				return false;\
			};\
			val.onStart = [](EqHeap::Handler handler, Object* owner)->bool {\
				auto heap = Singleton<EqHeap>::GetInstance();\
				if (auto p = heap->Ref<NAME>(handler))\
				{\
					return p->OnStart(owner);\
				}\
				return false;\
			};\
			val.fixedUpdate = [](EqHeap::Handler handler, Object* owner)->bool {\
				auto heap = Singleton<EqHeap>::GetInstance();\
				if (auto p = heap->Ref<NAME>(handler))\
				{\
					return p->FixedUpdate(owner);\
				}\
				return false;\
			};\
			Singleton<ScriptMapper>::GetInstance()->Register(val);\
		}\
		static size_t Hash(){ return m_hash; }\
	private:\
		static size_t m_hash;\
	};\
	size_t EQ_SCRIPT_REGISTER_NAME(NAME)::m_hash = 0;\
	static EQ_SCRIPT_REGISTER_NAME(NAME) EQ_SCRIPT_REGISTER_NAME(NAME)##inst;\

#endif
