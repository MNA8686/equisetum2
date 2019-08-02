#include "Node.hpp"

using namespace Equisetum2;

/*
�ŏ��ɃC���f�b�N�X�ԍ�0�̓��ʂ�root�m�[�h���쐬�����B
�e��ݒ肵�Ă��Ȃ��A�܂��͐e�����ł����S�Ẵm�[�h��root�m�[�h���Q�Ƃ���B
�e�������Ă��Ȃ��̂�root�m�[�h�݂̂ł���B
*/

class EqHeap
{
public:
	using Handler = uint32_t;
	friend Singleton<EqHeap>;	// �V���O���g������C���X�^���X���쐬���Ă��炦��悤�ɂ���

	template<typename T>
	class Container
	{
	public:
		explicit Container() = default;

		explicit Container(Handler handler)
		{
			auto heap = Singleton<EqHeap>::GetInstance();

			if (heap->AddRef(handler) > 0)
			{
				m_handler = handler;

				if (std::is_class<T>::value)
				{
					CallConstructor(Ref(), heap->Size(handler) / sizeof(T));
				}
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
				if (std::is_class<T>::value)
				{
					CallDestructor(Ref(), heap->Size(m_handler) / sizeof(T));
				}

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
				if (std::is_class<T>::value)
				{
					CallDestructor(Ref(), heap->Size(m_handler) / sizeof(T));
				}

				heap->Delete(m_handler);
			}
		}

		operator Handler () const
		{
			return m_handler;
		}

	private:
		void CallConstructor(T* ref, uint32_t num)
		{
			for (uint32_t i = 0; i < num; i++)
			{
				new (ref) T();
				ref++;
			}
		}

		void CallDestructor(T* ref, uint32_t num)
		{
			for (uint32_t i = 0; i < num; i++)
			{
				ref->~T();
				ref++;
			}
		}

		Handler m_handler = 0;
	};

	// �ア�Q�Ɨp�R���e�i
	template<typename T>
	class WeakRef
	{
	public:
		explicit WeakRef() = default;

		WeakRef(const Container<T>& src)
		{
			auto heap = Singleton<EqHeap>::GetInstance();

			if (heap->Test(src.m_handler))
			{
				m_handler = src.m_handler;
			}
		}

		WeakRef<T>& operator =(const Container<T>& src)
		{
			auto heap = Singleton<EqHeap>::GetInstance();

			if (heap->Test(src))
			{
				m_handler = src;
			}

			return (*this);
		}

		T* Ref()
		{
			auto heap = Singleton<EqHeap>::GetInstance();
			return heap->Ref<T>(m_handler);
		}

		~WeakRef() = default;

		operator Handler () const
		{
			return m_handler;
		}

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

	int32_t AddRef(Handler handler);
	int32_t DecRef(Handler handler);

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
	Handler New(uint32_t allocSize);
	void Delete(Handler handler);
	bool Swap(Handler handler1, Handler handler2);

	template<typename T>
	T* Ref(Handler handler) const
	{
		return static_cast<T*>(Ref(handler));
	}
	
	int32_t UsedHandlerNum() const;
	int32_t Size(Handler handler) const;
	void* Ref(Handler handler) const;
	bool Test(Handler handler) const;

	bool Load(std::shared_ptr<IStream> in);
	bool Save(std::shared_ptr<IStream> out);

	template<class T>
	struct isContainer : std::false_type {};

	template<class T>
	struct isContainer<Container<T>> : std::true_type {};

};


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
			//memset(slot.id, 0, sizeof(slot.id));

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
	//std::vector<int32_t> v;

	TestStruct() = default;
	~TestStruct() = default;
};

#if 1
template<typename T>
class EqVector final
{
public:
	class iterator
	{
	private:
		T* m_ptr = nullptr;
		int32_t m_count = 0;

	public:

		iterator(T* ptr, int32_t count)
		{
			m_ptr = ptr;
			m_count = count;
		}

		iterator& operator++()
		{
			m_count++;

			return *this;
		}

		iterator operator++(int)
		{
			auto it = *this;

			++(*this);

			return it;
		}

		const T& operator*() const
		{
			return m_ptr[m_count];
		}

		const T* operator->() const
		{
			return &m_ptr[m_count];
		}

		bool operator== (const iterator& r) const
		{
			return m_count == r.m_count;
		}

		bool operator!= (const iterator& r) const
		{
			return !(m_count == r.m_count);
		}
	};

	EqVector() = default;
	EqVector(int32_t num)
	{
		Resize(num);
	}

	~EqVector()
	{
		auto heap = Singleton<EqHeap>::GetInstance();

		Clear();

		heap->Delete(m_handler);
	}

	EqVector<T>& operator =(const EqVector<T>& src)
	{
		auto heap = Singleton<EqHeap>::GetInstance();

		if (src.m_reservedSize > 0)
		{
			EqHeap::Handler handler = heap->New(src.m_reservedSize * sizeof(T));
			if (handler != 0)
			{
				if (std::is_class<T>::value)
				{
					//�R�s�[�R���X�g���N�^�Ăяo��
					CallCopyConstructor(reinterpret_cast<T*>(heap->Ref(handler)), src.Data(), src.m_usedSize);
				}
				else
				{
					memcpy(heap->Ref(handler), src.Data(), src.m_usedSize * sizeof(T));
				}

				m_handler = handler;
				m_reservedSize = src.m_reservedSize;
				m_usedSize = src.m_usedSize;
			}
		}

		return (*this);
	}

	void Reserve(int32_t reserveSize)
	{
		auto heap = Singleton<EqHeap>::GetInstance();

		if (m_reservedSize < reserveSize)
		{
			EqHeap::Handler handler = heap->New(reserveSize * sizeof(T));

			if (handler != 0)
			{
				if (std::is_class<T>::value)
				{
					//�R�s�[�R���X�g���N�^�Ăяo��
					CallCopyConstructor(reinterpret_cast<T*>(heap->Ref(handler)), Data(), m_usedSize);

					// �Â����̔z��ɓ����Ă���I�u�W�F�N�g�̃f�X�g���N�^���Ă�
					CallDestructor(Data(), m_usedSize);
				}
				else
				{
					memcpy(heap->Ref(handler), Data(), m_usedSize * sizeof(T));
				}

				m_reservedSize = reserveSize;

				if (m_handler != 0)
				{
					// �V�����m�ۂ����������ƌÂ������������ւ���
					heap->Swap(m_handler, handler);
					// �Â��ق��̃��������폜
					heap->Delete(handler);
				}
				else
				{
					m_handler = handler;
				}
			}
		}
	}

	void Resize(int32_t size)
	{
		if (m_usedSize != size)
		{
			if (m_usedSize > size)
			{
				if (std::is_class<T>::value)
				{
					// ����������ꍇ�͂͂����o����镔�������f�X�g���N�^���Ă�
					CallDestructor(&Data()[size], m_usedSize - size);
				}

				// ���g�p�ɂȂ����̈���[������
				memset(&Data()[size], 0, (m_usedSize - size) * sizeof(T));
			}
			else
			{
				if (size > m_reservedSize)
				{
					Reserve(NextSize(size));
				}
				
				if (std::is_class<T>::value)
				{
					// �傫���Ȃ����������R���X�g���N�^���Ă�
					CallConstructor(&Data()[m_usedSize], size - m_usedSize);
				}
			}
			m_usedSize = size;
		}
	}

	T* Data()
	{
		auto heap = Singleton<EqHeap>::GetInstance();
		return heap->Ref<T>(m_handler);
	}

	const T* Data() const 
	{
		auto heap = Singleton<EqHeap>::GetInstance();
		return heap->Ref<T>(m_handler);
	}

	void Clear()
	{
		Resize(0);
	}

	void PushBack(const T& src)
	{
		// �܂��̓����������m�ۂ���
		int32_t destSize = m_usedSize + 1;
		if (destSize > m_reservedSize)
		{
			Reserve(NextSize(destSize));
		}

		if (std::is_class<T>::value)
		{
			CallCopyConstructor(&Data()[m_usedSize], &src, 1);
		}
		else
		{
			Data()[m_usedSize] = src;
		}

		m_usedSize++;
	}

	void PopBack()
	{
		if (m_usedSize > 0)
		{
			Resize(m_usedSize - 1);
		}
	}

	int32_t Size() const
	{
		return m_usedSize;
	}

	iterator begin()
	{
		return{ Data(), 0 };
	}

	iterator end()
	{
		return{ Data(), m_usedSize };
	}

private:
	void CallConstructor(T* ref, uint32_t num)
	{
		for (uint32_t i = 0; i < num; i++)
		{
			new (ref) T();
			ref++;
		}
	}
	
	void CallCopyConstructor(T* destRef, const T* srcRef, uint32_t num)
	{
		for (uint32_t i = 0; i < num; i++)
		{
			new (destRef) T(*srcRef);
			srcRef++;
			destRef++;
		}
	}

	void CallDestructor(T* ref, uint32_t num)
	{
		for (uint32_t i = 0; i < num; i++)
		{
			ref->~T();
			ref++;
		}
	}

	int32_t NextSize(int32_t size) const
	{
		int32_t destSize = 16;

		while (destSize < size)
		{
			destSize *= 2;
		}

		return destSize;
	}

	int32_t m_reservedSize = 0;
	int32_t m_usedSize = 0;
	EqHeap::Handler m_handler = 0;
};
#endif


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

	int32_t size = sizeof(TestStruct);
	printf("%d", size);

#if 1
	{
		EqVector<TestClass> v(4);
		EqVector<TestClass> v2;

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


