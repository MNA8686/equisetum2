#include "Node.hpp"

using namespace Equisetum2;

/*
最初にインデックス番号0の特別なrootノードが作成される。
親を設定していない、または親が消滅した全てのノードはrootノードを参照する。
親を持っていないのはrootノードのみである。
*/


class EqHeap
{
public:
	using Handler = uint32_t;

	template<typename T>
	class Container
	{
	public:
		explicit Container() = default;

		explicit Container(Handler handler)
		{
			m_handler = handler;
		}

		T* Ref(std::shared_ptr<EqHeap>& heapSystem)
		{
			return heapSystem->Ref<T>(m_handler);
		}

		~Container()
		{
			printf("");
		}

	private:
		friend class EqHeap;
		Handler m_handler = 0;
	};

protected:

	typedef struct
	{
		Handler handler;
		uint32_t size;
		void* ptr;
	}stHeap;

	std::vector<stHeap> m_vHandler;
	int32_t m_reservedSize = 0;

	Handler New(uint32_t allocSize);
	void* Ref(Handler handler);

	template<typename T>
	T* Ref(Handler handler)
	{
		return static_cast<T*>(Ref(handler));
	}

	void Delete(Handler handler);

public:
	static std::shared_ptr<EqHeap> Create(int32_t maxHandlerSize, int32_t reservedSize=0);
	EqHeap() = default;
	~EqHeap();

	template<typename T>
	Container<T> New(uint32_t num=1)
	{
		auto handler = New(sizeof(T) * num);
		return Container<T>(handler);
	}
	
	template<typename T>
	void Delete(Container<T>& cont)
	{
		Delete(cont.m_handler);
	}

	bool Load(std::shared_ptr<IStream> in);
	bool Save(std::shared_ptr<IStream> out);
};
// オートリリーサー

#if 0
class EqHeapAccessor
{
public:

	friend Singleton<EqHeapAccessor>;	// シングルトンからインスタンスを作成してもらえるようにする

	EqHeapAccessor() = default;
	~EqHeapAccessor() = default;

	int32_t Set(std::shared_ptr<EqHeap>& heapSysmtem);
	EqHeap* Get(int32_t id);

private:

	//std::vector<std::shared_ptr<EqHeap>> m_vHeap;
	std::shared_ptr<EqHeap> m_heap;
};

int32_t EqHeapAccessor::Set(std::shared_ptr<EqHeap>& heapSysmtem)
{
	m_heap = heapSysmtem;
	return 0;
}
#endif

EqHeap::~EqHeap()
{
	for (auto& slot : m_vHandler)
	{
		if (slot.ptr)
		{
			free(slot.ptr);
		}
	}
}

std::shared_ptr<EqHeap> EqHeap::Create(int32_t maxHandlerSize, int32_t reservedSize)
{
	uint32_t index = 0;

	auto p = std::make_shared<EqHeap>();
	if (p)
	{

		p->m_vHandler.resize(maxHandlerSize);

		for (auto& slot : p->m_vHandler)
		{
			// 上位16ビットはハンドラの番号、下位16ビットはシリアル番号
			slot.handler = (index << 16) | 1;
			slot.size = 0;
			slot.ptr = nullptr;

			index++;
		}

		p->m_reservedSize = reservedSize;
	}

	return p;
}

EqHeap::Handler EqHeap::New(uint32_t allocSize)
{
	Handler handler = 0;

	if (allocSize != 0)
	{
		for (auto& slot : m_vHandler)
		{
			if (!slot.ptr)
			{
				slot.ptr = malloc(allocSize);
				slot.size = allocSize;
				memset(slot.ptr, 0, allocSize);
				handler = slot.handler;
				break;
			}
		}
	}

	return handler;
}

void* EqHeap::Ref(Handler handler)
{
	if (handler != 0)
	{
		uint32_t index = handler >> 16;
		auto& slot = m_vHandler[index];

		if (slot.handler == handler)
		{
			return slot.ptr;
		}
	}

	return nullptr;
}

void EqHeap::Delete(Handler handler)
{
	if (handler != 0)
	{
		uint32_t index = handler >> 16;
		auto& slot = m_vHandler[index];

		if (slot.handler == handler)
		{
			free(slot.ptr);
			slot.ptr = nullptr;
			slot.size = 0;
			//memset(slot.id, 0, sizeof(slot.id));

			// 次回のためにシリアル番号をインクリメント(0は避ける)
			uint32_t nextSerial = ((handler + 1) & 0x0000ffff);
			if (nextSerial == 0)
			{
				nextSerial = 1;
			}

			slot.handler = (handler & 0xffff0000) | nextSerial;
		}
	}
}


class TestStruct
{
public:
	EqHeap::Container<int32_t> i;
	float f;
	EqHeap::Container<int32_t> i2;

	TestStruct() = default;
	~TestStruct() = default;
};

void heap_test()
{
	auto heap = EqHeap::Create(65536);

#if 0
	struct testst
	{
		EqHeap::Container<int32_t> h1;
		EqHeap::Container<int32_t> h2;
		char arr[128];
	};
#endif

	int32_t size = sizeof(TestStruct);
	printf("%d", size);

	{
		//TestStruct st;

		auto h = heap->New<TestStruct>();

		auto p = h.Ref(heap);
		p->~TestStruct();

//		st.i = heap->New<int32_t>();
//		auto ref = st.i.Ref(heap);
//		*ref = 123;

		//	heap->Delete(h1);
		
		//auto ref2 = h1.Ref(heap);
		//*ref2 = 123;
	}

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


