#include "Node.hpp"

using namespace Equisetum2;

/*
最初にインデックス番号0の特別なrootノードが作成される。
親を設定していない、または親が消滅した全てのノードはrootノードを参照する。
親を持っていないのはrootノードのみである。
*/

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

void heap_test()
{
	EqHeap::Initialize();

	auto p1 = EqHeap::New(16, "testdayontestdayontestdayontestdayontestdayontestdayon");
	auto p2 = EqHeap::New(16, "");
	auto p3 = EqHeap::New(16, "doraemon");
	auto p4 = EqHeap::New(16, "neko");

	EqHeap::Delete(p2);
	EqHeap::Delete(p2);
	auto p5 = EqHeap::New(16, "neko");

	EqHeap::Finalize();
}

