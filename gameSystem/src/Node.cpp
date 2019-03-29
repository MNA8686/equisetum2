#include "Node.hpp"

using namespace Equisetum2;

/*
�ŏ��ɃC���f�b�N�X�ԍ�0�̓��ʂ�root�m�[�h���쐬�����B
�e��ݒ肵�Ă��Ȃ��A�܂��͐e�����ł����S�Ẵm�[�h��root�m�[�h���Q�Ƃ���B
�e�������Ă��Ȃ��̂�root�m�[�h�݂̂ł���B
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
			// ���16�r�b�g�̓n���h���̔ԍ��A����16�r�b�g�̓V���A���ԍ�
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

				// ����̂��߂ɃV���A���ԍ����C���N�������g(0�͔�����)
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

