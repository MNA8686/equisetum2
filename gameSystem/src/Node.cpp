#include "Node.hpp"

using namespace Equisetum2;

/*
最初にインデックス番号0の特別なrootノードが作成される。
親を設定していない、または親が消滅した全てのノードはrootノードを参照する。
親を持っていないのはrootノードのみである。
*/

/*
namespace EqHeap
{
	using Handler = uint32_t;

	const uint32_t maxSize = 3;// 65536;

	typedef struct
	{
		char id[32];
		Handler handler;
		uint32_t size;
		void* ptr;
	}stHeap;

	static stHeap g_heap[maxSize];

	bool Initialize();
	bool Finalize();

	Handler New(uint32_t size, const String name = "");
	void* Ref(Handler handler);
	void Delete(Handler handler);

	bool Initialize()
	{
		uint32_t index = 0;

		memset(g_heap, 0, sizeof(g_heap));

		for (auto& slot : g_heap)
		{
			// 上位16ビットはハンドラの番号、下位16ビットはシリアル番号
			slot.handler = (index << 16) | 1;
			slot.size = 0;
			slot.ptr = nullptr;

			index++;
		}

		return true;
	}

	Handler New(uint32_t size, const String name)
	{
		Handler handler = 0;

		if (size != 0)
		{
			for (auto& slot : g_heap)
			{
				if (!slot.ptr)
				{
					slot.ptr = malloc(size);
					slot.size = size;
					strncpy(slot.id, name.c_str(), sizeof(slot.id) - 1);
					handler = slot.handler;
					break;
				}
			}
		}

		return handler;
	}

	void* Ref(Handler handler)
	{
		if (handler != 0)
		{
			uint32_t index = handler >> 16;
			auto& slot = g_heap[index];

			if (slot.handler == handler)
			{
				return slot.ptr;
			}
		}

		return nullptr;
	}

	void Delete(Handler handler)
	{
		if (handler != 0)
		{
			uint32_t index = handler >> 16;
			auto& slot = g_heap[index];

			if (slot.handler == handler)
			{
				free(slot.ptr);
				slot.ptr = nullptr;
				slot.size = 0;
				memset(slot.id, 0, sizeof(slot.id));

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

	bool Finalize()
	{
		for (auto& slot : g_heap)
		{
			if (slot.ptr)
			{
				free(slot.ptr);
			}
		}

		return true;
	}
}
*/

class EqHeap
{
protected:

	using Handler = uint32_t;

	typedef struct
	{
		Handler handler;
		uint32_t size;
		void* ptr;
	}stHeap;

	std::vector<stHeap> m_vHandler;

public:
	static std::shared_ptr<EqHeap> Create(int32_t maxHandlerSize);
	EqHeap() = default;
	~EqHeap();

	Handler New(uint32_t allocSize);

	template<typename T>
	Handler New()
	{
		return New(sizeof(T));
	}

	void* Ref(Handler handler);
	
	template<typename T>
	T* Ref(Handler handler)
	{
		return static_cast<T*>(Ref(handler));
	}

	void Delete(Handler handler);

	bool Load(std::shared_ptr<IStream> in);
	bool Save(std::shared_ptr<IStream> out);
};
// オートリリーサー

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

std::shared_ptr<EqHeap> EqHeap::Create(int32_t maxHandlerSize)
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
				//strncpy(slot.id, name.c_str(), sizeof(slot.id) - 1);
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

void heap_test()
{
	auto heap = EqHeap::Create(65536);

	auto h1 = heap->New<int32_t>();
	auto ref = heap->Ref<int32_t>(h1);
	*ref = 123;


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

