#include "EqHeap.hpp"

EqHeap::~EqHeap()
{
	// ������Ɖ�����s���Ă���΂����ł�邱�Ƃ͖����͂��ł���
	// ���������[�N���o���s���H
	//InitHeapSystem(0);

	if (UsedHandlerNum() > 0)
	{
		Logger::OutputDebug("memory leak!");
	}
}

bool EqHeap::InitHeapSystem(int32_t maxHandlerSize, int32_t reservedSize)
{
	// �S���
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
		// ���16�r�b�g�̓n���h���̔ԍ��A����16�r�b�g�̓V���A���ԍ�
		slot.handler = (index << 16) | 1;
		//slot.size = 0;
		//slot.refCount = 0;
		//slot.ptr = nullptr;

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
				//slot.refCount = 0;
				memset(slot.ptr, 0, allocSize);
				handler = slot.handler;
				m_usedNum++;
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

// �n���h���Ɋ���t�����Ă��郁�����̃T�C�Y
int32_t EqHeap::Size(Handler handler) const
{
	if (handler != 0)
	{
		uint32_t index = handler >> 16;
		auto& slot = m_vHandler[index];

		if (slot.handler == handler)
		{
			return slot.size;
		}
	}

	return 0;
}

// �n���h�����L�����ǂ������ׂ�
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

bool EqHeap::Load(std::shared_ptr<IStream> in, EqHeap::Handler& hint)
{
	if (in->CanRead())
	{
		// �q���g
		in->Read(reinterpret_cast<uint8_t*>(&hint), sizeof(hint));

		for (auto& handler : m_vHandler)
		{
			if (handler.ptr)
			{
				free(handler.ptr);
			}

			// �n���h��
			in->Read(reinterpret_cast<uint8_t*>(&handler.handler), sizeof(handler.handler));
			// �Q�ƃJ�E���^
			in->Read(reinterpret_cast<uint8_t*>(&handler.refCount), sizeof(handler.refCount));
			// �T�C�Y
			in->Read(reinterpret_cast<uint8_t*>(&handler.size), sizeof(handler.size));

			// �f�[�^����
			if (handler.size > 0)
			{
				handler.ptr = malloc(handler.size);
				in->Read(reinterpret_cast<uint8_t*>(handler.ptr), handler.size);
			}
		}

		return true;
	}

	return false;
}

bool EqHeap::Save(std::shared_ptr<IStream> out, EqHeap::Handler hint)
{
	if (out->CanWrite())
	{
		// �q���g
		out->Write(reinterpret_cast<uint8_t*>(&hint), sizeof(hint));

		for (auto& handler : m_vHandler)
		{
			// �n���h��
			out->Write(reinterpret_cast<uint8_t*>(&handler.handler), sizeof(handler.handler));
			// �Q�ƃJ�E���^
			out->Write(reinterpret_cast<uint8_t*>(&handler.refCount), sizeof(handler.refCount));
			// �T�C�Y
			out->Write(reinterpret_cast<uint8_t*>(&handler.size), sizeof(handler.size));

			// �f�[�^����
			if (handler.ptr)
			{
				out->Write(reinterpret_cast<uint8_t*>(handler.ptr), handler.size);
			}
		}

		return true;
	}

	return false;
}

// ���̂�����ւ���
bool EqHeap::Swap(Handler handler1, Handler handler2)
{
	if (handler1 == handler2)
	{
		return true;
	}

	if (!Test(handler1) || !Test(handler2))
	{
		return false;
	}

	uint32_t index1 = handler1 >> 16;
	auto& slot1 = m_vHandler[index1];
	uint32_t index2 = handler2 >> 16;
	auto& slot2 = m_vHandler[index2];

	std::swap(slot1.ptr, slot2.ptr);
	std::swap(slot1.size, slot2.size);

	return true;
}

// �n���h�����폜���A���������������
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
			slot.refCount = 0;
			//memset(slot.id, 0, sizeof(slot.id));

			// ����̂��߂ɃV���A���ԍ����C���N�������g(0�͔�����)
			uint32_t nextSerial = ((handler + 1) & 0x0000ffff);
			if (nextSerial == 0)
			{
				nextSerial = 1;
			}

			slot.handler = (handler & 0xffff0000) | nextSerial;
			m_usedNum--;
		}
	}
}

int32_t EqHeap::UsedHandlerNum() const
{
	return m_usedNum;
}

