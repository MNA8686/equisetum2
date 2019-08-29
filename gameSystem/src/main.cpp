#include "Equisetum2.h"
#include "Application.hpp"
#include "EqHeap.hpp"

using namespace Equisetum2;

int Main()
{
	Singleton<EqHeap>::GetInstance();

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
