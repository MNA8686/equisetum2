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
	friend Singleton<EqHeap>;	// シングルトンからインスタンスを作成してもらえるようにする

	template<typename T>
	class Container
	{
	public:
		explicit Container() = default;

		explicit Container(Handler handler)
		{
			if (Singleton<EqHeap>::GetInstance()->AddRef(handler) > 0)
			{
				m_handler = handler;
			}
		}

		Container(const Container<T>& src)
		{
			auto heap = Singleton<EqHeap>::GetInstance();

			if (heap->Test(src.m_handler))
			{
				heap->AddRef(src.m_handler);
				m_handler = src.m_handler;
			}
		}

		Container<T>& operator =(const Container<T>& src)
		{
			auto heap = Singleton<EqHeap>::GetInstance();

			if (heap->DecRef(m_handler) == 0)
			{
				auto ref = Ref();
				heap->CallDestructer<T>(ref);

				heap->Delete(m_handler);
			}

			if (heap->AddRef(src.m_handler) > 0)
			{
				m_handler = src.m_handler;
			}
			else
			{
				m_handler = 0;
			}

			return (*this);
		}

		//T* Ref(std::shared_ptr<EqHeap>& heapSystem)
		T* Ref()
		{
			auto heap = Singleton<EqHeap>::GetInstance();
			return heap->Ref<T>(m_handler);
		}

		~Container()
		{
			auto heap = Singleton<EqHeap>::GetInstance();
			if (heap->DecRef(m_handler) == 0)
			{
				auto ref = Ref();
				CallDestructer<T>(ref);

				heap->Delete(m_handler);
			}
		}

		operator Handler () const
		{
			return m_handler;
		}

		//template<typename T>
		void CallDestructer(T* ref)
		{
			return;
		}
		
	#if 1
		template < <typename T2> > void CallDestructer(Container<T2>* ref)
		{
			ref->~Container();
		}
	#endif

	private:
		Handler m_handler = 0;
	};

protected:

	typedef struct
	{
		Handler handler;
		uint32_t size;
		int32_t refCount;
		void* ptr;
	}stHeap;

	std::vector<stHeap> m_vHandler;
	int32_t m_reservedSize = 0;

	Handler New(uint32_t allocSize);
	void* Ref(Handler handler) const;

	int32_t AddRef(Handler handler);
	int32_t DecRef(Handler handler);

	template<typename T>
	T* Ref(Handler handler) const
	{
		return static_cast<T*>(Ref(handler));
	}

	bool Test(Handler handler) const;


	void Delete(Handler handler);

public:
	bool InitHeapSystem(int32_t maxHandlerSize, int32_t reservedSize=0);

	EqHeap() = default;
	~EqHeap();

	template<typename T>
	Container<T> New(uint32_t num=1)
	{
		auto handler = New(sizeof(T) * num);
		return Container<T>(handler);
	}
	
#if 0
	template<typename T>
	void Delete(Container<T>& cont)
	{
		Delete(cont.m_handler);
	}
#endif

	int32_t UsedHandlerNum() const;

	bool Load(std::shared_ptr<IStream> in);
	bool Save(std::shared_ptr<IStream> out);
};

#if 0
template<>
template<class T> EqHeap::Container<EqHeap::Container<T>>::~Container()
{
	auto heap = Singleton<EqHeap>::GetInstance();
	if (heap->DecRef(m_handler) == 0)
	{
		auto ref = heap->Ref();
		ref->~Container();

		heap->Delete(m_handler);
	}
}
#endif

EqHeap::~EqHeap()
{
	InitHeapSystem(0);
}

bool EqHeap::InitHeapSystem(int32_t maxHandlerSize, int32_t reservedSize)
{
	// 全解放
	for (auto& slot : m_vHandler)
	{
		if (slot.ptr)
		{
			free(slot.ptr);
		}
	}

	m_vHandler.resize(maxHandlerSize);

	uint32_t index = 0;

	for (auto& slot : m_vHandler)
	{
		// 上位16ビットはハンドラの番号、下位16ビットはシリアル番号
		slot.handler = (index << 16) | 1;
		slot.size = 0;
		slot.refCount = 0;
		slot.ptr = nullptr;

		index++;
	}

	m_reservedSize = reservedSize;

	return true;
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

void* EqHeap::Ref(Handler handler) const
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

int32_t EqHeap::AddRef(Handler handler)
{
	if (handler != 0)
	{
		uint32_t index = handler >> 16;
		auto& slot = m_vHandler[index];

		if (slot.handler == handler)
		{
			slot.refCount++;
			return slot.refCount;
		}
	}
	
	return -1;
}

int32_t EqHeap::DecRef(Handler handler)
{
	if (handler != 0)
	{
		uint32_t index = handler >> 16;
		auto& slot = m_vHandler[index];

		if (slot.handler == handler)
		{
			slot.refCount--;
			return slot.refCount;
		}
	}
	
	return -1;
}

bool EqHeap::Test(Handler handler) const
{
	if (handler != 0)
	{
		uint32_t index = handler >> 16;
		auto& slot = m_vHandler[index];

		if (slot.handler == handler)
		{
			return true;
		}
	}

	return false;
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

int32_t EqHeap::UsedHandlerNum() const
{
	int32_t num = 0;

	for (auto& slot : m_vHandler)
	{
		if (slot.ptr != 0)
		{
			num++;
		}
	}

	return num;
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
	auto heap = Singleton<EqHeap>::GetInstance();
	heap->InitHeapSystem(32768);

	int32_t size = sizeof(TestStruct);
	printf("%d", size);

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
#if 1
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


