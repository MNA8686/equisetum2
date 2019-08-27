#include "Equisetum2.h"
#include "Application.hpp"
#include "EqHeap.hpp"

using namespace Equisetum2;


class ScriptMapper final
{
public:
	friend Singleton<ScriptMapper>;	// シングルトンからインスタンスを作成してもらえるようにする

	struct Value
	{
		String id;
		size_t hash = 0;
		size_t instanceSize = 0;
		std::function<bool (EqHeap::Handler handler)> constructor;
		std::function<bool (EqHeap::Handler handler)> destructor;
	};

	void Set(const Value& value);
	int32_t ToIndex(const String& id) const;
	const Value* Get(int32_t index) const;

private:
	std::vector<Value> m_vValue;
};

void ScriptMapper::Set(const Value& value)
{
	m_vValue.push_back(value);
}

int32_t ScriptMapper::ToIndex(const String& id) const
{
	int32_t index = 0;

	for (auto& val : m_vValue)
	{
		if (id == val.id)
		{
			return index;
		}

		index++;
	}

	return -1;
}

const ScriptMapper::Value* ScriptMapper::Get(int32_t index) const
{
	if (index >= 0 && index < static_cast<int32_t>(m_vValue.size()))
	{
		return &m_vValue[index];
	}

	return nullptr;
}

#define EQ_SCRIPT_REGISTER(NAME) \
	class NAME##_register\
	{\
	public:\
		NAME##_register()\
		{\
			ScriptMapper::Value val;\
			val.id = #NAME;\
			val.hash = std::hash<String>()(#NAME);\
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
			Singleton<ScriptMapper>::GetInstance()->Set(val);\
		}\
	};\
	static NAME##_register NAME##inst;\


class Testdayo
{
public:
	int a;
};
EQ_SCRIPT_REGISTER(Testdayo);

#if 1
class ScriptContainer
{
public:
	ScriptContainer() = default;
	~ScriptContainer()
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

	static EqHeap::Container<ScriptContainer> Create(const String& scriptName)
	{
		auto heap = Singleton<EqHeap>::GetInstance();
		auto scriptMapper = Singleton<ScriptMapper>::GetInstance();

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
					// ハッシュを設定
					ref->m_hash = p->hash;

					// スクリプトのインスタンスを作成
					ref->m_ctx = heap->New(p->instanceSize);

					// スクリプトのコンストラクタを呼び出す
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

	template <class T>
	T* Ref()
	{
		auto heap = Singleton<EqHeap>::GetInstance();
		auto scriptMapper = Singleton<ScriptMapper>::GetInstance();

		if (const ScriptMapper::Value* value = scriptMapper->Get(m_index))
		{
			if (auto ref = heap->Ref<T>(m_ctx))
			{
				// TODO これはあかん
				// 型が合っているかチェック
				if (ref->GetHash() == value->hash)
				{
					return ref;
				}
			}
		}

		return nullptr;
	}


private:

	size_t GetHash() const
	{
		return m_hash;
	}

	EqHeap::Handler m_ctx = 0;	// スクリプトの実体
	int32_t m_index = -1;		// スクリプトマッパーのインデックス
	size_t m_hash = 0;			// キャスト時判定用ハッシュ
};
#endif

int Main()
{
	Singleton<EqHeap>::GetInstance();

	{
		auto index = Singleton<ScriptMapper>::GetInstance()->ToIndex("Test");
		if (index >= 0)
		{
		}
		auto index2 = Singleton<ScriptMapper>::GetInstance()->ToIndex("Testdayo");
		if (index2 >= 0)
		{
		}
	}

	if(0)
	{
		class Test final : public INodeAttachment
		{
		public:
		};

		auto heap = Singleton<EqHeap>::GetInstance();
		heap->InitHeapSystem(32768);

#if 0
		{
			auto ctx = Singleton<NodePool<Test>>::GetInstance()->CreateContext();
			Singleton<NodePool<Test>>::GetInstance()->SetContext(ctx);

			auto main = Node<Test>::GetFreeNodeWithInit("main");

			auto child1 = Node<Test>::GetFreeNodeWithInit("child1");
			auto node_child1 = Node<Test>::GetNodeByHandler(child1);
			node_child1->SetParentHandler(main);

			auto child2 = Node<Test>::GetFreeNodeWithInit("child2");
			auto node_child2 = Node<Test>::GetNodeByHandler(child2);
			node_child2->SetParentHandler(main);

			auto child3 = Node<Test>::GetFreeNodeWithInit("child3");
			auto node_child3 = Node<Test>::GetNodeByHandler(child3);
			node_child3->SetParentHandler(main);

			Node<Test>::Dump();

			auto file = FileStream::NewFileFromPath("test.bin");
			heap->Save(file, ctx);
		}
#else
		{
			EqHeap::Handler handler;
			auto file = FileStream::CreateFromPath("test.bin");
			heap->Load(file, handler);

			EqHeap::Container<NodePool<Test>::Context> ctx;
			ctx.Wrap(handler);
			Singleton<NodePool<Test>>::GetInstance()->SetContext(ctx);

			Node<Test>::Dump();
		}
#endif
	}

	if (auto app = GetApplication())
	{
		return app->Main();
	}

	return -1;
}
