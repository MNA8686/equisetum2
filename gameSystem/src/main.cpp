#include "Equisetum2.h"
#include "Application.hpp"
#include "EqHeap.hpp"

using namespace Equisetum2;

#if 0
class ScriptBinder
{
public:
	ScriptBinder() = default;
	~ScriptBinder()
	{
		auto heap = Singleton<EqHeap>::GetInstance();

		if(heap->DecRef(m_ctx) == 0)
		{
			Destruct(*this);
			heap->Delete(m_ctx);
		}
	}

	static EqHeap::Container<ScriptBinder> Create(const String& scriptName)
	{
		auto heap = Singleton<EqHeap>::GetInstance();

		auto inst = heap->New<ScriptBinder>();
		if(auto ref = inst.Ref())
		{
//			ref->m_ctx;

			return inst;
		}

		return {};
	}

	template <class T>
	T* Ref()
	{
		if(T::hash() == m_hash)
		{
			return Singleton<EqHeap>::GetInstance()->Ref<T>(m_ctx);
		}

		return nullptr;
	}


	static void Destruct(ScriptBinder& inst)
	{
		// ハッシュに対応するデストラクタを呼び出す
	}

private:
	EqHeap::Container<char> m_name;		// スクリプト名
	EqHeap::Handler m_ctx;			// スクリプトの実体
	size_t m_hash = 0;			// キャスト時判定用ハッシュ
};
#endif


int Main()
{
	Singleton<EqHeap>::GetInstance();

	if(1)
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
