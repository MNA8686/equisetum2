#include "Node.hpp"

using namespace Equisetum2;

/*
最初にインデックス番号0の特別なrootノードが作成される。
親を設定していない、または親が消滅した全てのノードはrootノードを参照する。
親を持っていないのはrootノードのみである。
*/

#include "EqHeap.hpp"
#include "EqVector.hpp"

static int testCount = 0;
class TestClass
{
public:
	TestClass()
	{
		Logger::OutputDebug("con %d", testCount);
		i = testCount;
		testCount++;
	}
	~TestClass()
	{
		Logger::OutputDebug("des %d", i);
	}
	TestClass(const TestClass& src)
	{
		Logger::OutputDebug("copy %d", src.i);
		i = src.i;
	}

	int i = 0;
};

void heap_test()
{
	auto heap = Singleton<EqHeap>::GetInstance();
	heap->InitHeapSystem(32768);

	int size = 0;

#if 1
	{
		EqVector<TestClass> v(0);
		EqVector<TestClass> v2(2);

		{
			Logger::OutputDebug("v size %d", v.Size());
			Logger::OutputDebug("v2 size %d", v2.Size());

			v2 = v;
			Logger::OutputDebug("v2 size %d", v2.Size());

			v.Clear();
			Logger::OutputDebug("v size %d", v.Size());
			Logger::OutputDebug("v2 size %d", v2.Size());
			
			v2.PushBack({});
			Logger::OutputDebug("v size %d", v.Size());
			Logger::OutputDebug("v2 size %d", v2.Size());
		}
	}
#endif
#if 0
	{
		EqVector<TestClass> v(4);

		{
			Logger::OutputDebug("");
			v.Resize(6);
		}

		Logger::OutputDebug("");
		TestClass tmp;
		v.PushBack(tmp);

		Logger::OutputDebug("");
		v.PopBack();
		v.PopBack();
		v.PopBack();
		v.PopBack();

		Logger::OutputDebug("");
		v.Resize(2);

		Logger::OutputDebug("");
		v.Resize(5);

		Logger::OutputDebug("");
		v.Clear();

		Logger::OutputDebug("");
		v.Resize(20);

		size = sizeof(EqVector<int32_t>);
		Logger::OutputDebug("\n%d", size);
	}
#endif
#if 1
	{
		EqVector<int32_t> v(4);

		auto p = v.Data();
		p[0] = 123;
		p[1] = 456;
		p[2] = 789;
		p[3] = 888;

		for (auto& e : v)
		{
			auto num = e;
			Logger::OutputDebug("%d", num);
		}
		
		{
			Logger::OutputDebug("");
			v.Resize(6);
			auto p = v.Data();
			p[4] = 111;
			p[5] = 222;

			for (auto& e : v)
			{
				auto num = e;
				Logger::OutputDebug("%d", num);
			}
		}

		{
			Logger::OutputDebug("");
			v.PushBack(333);
			v.PushBack(444);

			for (auto& e : v)
			{
				auto num = e;
				Logger::OutputDebug("%d", num);
			}
		}

		Logger::OutputDebug("");
		v.Resize(2);
		for (auto& e : v)
		{
			auto num = e;
			Logger::OutputDebug("%d", num);
		}

		Logger::OutputDebug("");
		v.Resize(5);
		for (auto& e : v)
		{
			auto num = e;
			Logger::OutputDebug("%d", num);
		}

		size = sizeof(EqVector<int32_t>);
		Logger::OutputDebug("\n%d", size);
	}
#endif
#if 0
	{
		EqHeap::WeakRef<int32_t> w;
		EqHeap::WeakRef<int32_t> w2;

		{
			auto h = heap->New<int32_t>();
			w = h;

			size = heap->UsedHandlerNum();
			printf("%d", size);

			w2 = w;
			auto w_ref = w2.Ref();
			*w_ref = 123;

			auto h_ref = h.Ref();
			*h_ref = 123;
		}

		auto w_ref = w.Ref();

		size = heap->UsedHandlerNum();
		printf("%d", size);


		EqHeap::WeakRef<int32_t> arr[32];
		for (auto& e : arr)
		{

		}
	}
#endif
#if 0
	{
		auto h = heap->New<TestStruct>(2);
		auto h2 = heap->New<TestStruct>(1);
		//h.Ref()->v.resize(1024);
		//h2.Ref()->v.resize(1024);

		size = heap->UsedHandlerNum();
		printf("%d", size);
	}
#endif
#if 0
	{
		auto h = heap->New<int32_t>();
//		auto h2 = heap->New<int32_t>();

		size = heap->UsedHandlerNum();
		printf("%d", size);

		h = EqHeap::Container<int32_t>(0);

		size = heap->UsedHandlerNum();
		printf("%d", size);
	}
#endif
#if 0
	{
		auto h = heap->New<EqHeap::Container<int32_t>>();
		auto h2 = heap->New<int32_t>();

		auto ref = h.Ref();
		*ref = h2;

		auto ref2 = h2.Ref();
		*ref2 = 123;

		size = heap->UsedHandlerNum();
		printf("%d", size);
	}
#endif
#if 0
	{
		auto h = heap->New<int32_t>();
		auto h2 = h;

		auto ref = h2.Ref();
		*ref = 123;

		size = heap->UsedHandlerNum();
		printf("%d", size);
	}
#endif

	size = heap->UsedHandlerNum();
	printf("%d", size);

#if 0
	EqHeap::Initialize();

	auto p1 = EqHeap::New(16, "testdayontestdayontestdayontestdayontestdayontestdayon");
	auto p2 = EqHeap::New(16, "");
	auto p3 = EqHeap::New(16, "doraemon");
	auto p4 = EqHeap::New(16, "neko");

	EqHeap::Delete(p2);
	EqHeap::Delete(p2);
	auto p5 = EqHeap::New(16, "neko");

	EqHeap::Finalize();
#endif
}


