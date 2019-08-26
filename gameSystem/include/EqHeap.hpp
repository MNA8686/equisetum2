#if !defined(_EQHEAP_H_)
#define _EQHEAP_H_

#include "Equisetum2.h"
using namespace Equisetum2;

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
			Reset();
		}

		void Reset()
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

			m_handler = 0;
		}

		operator Handler () const
		{
			return m_handler;
		}

		void Wrap(Handler handler)
		{
			Reset();
			m_handler = handler;
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

	// 弱い参照用コンテナ
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
		Handler handler = 0;
		uint32_t size = 0;
		int32_t refCount = 0;
		void* ptr = nullptr;
	}stHeap;

	std::vector<stHeap> m_vHandler;
	int32_t m_reservedSize = 0;

	int32_t AddRef(Handler handler);
	int32_t DecRef(Handler handler);

public:
	bool InitHeapSystem(int32_t maxHandlerSize, int32_t reservedSize = 0);

	EqHeap() = default;
	~EqHeap();

	template<typename T>
	Container<T> New(uint32_t num = 1)
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

	bool Load(std::shared_ptr<IStream> in, Handler& hint);
	bool Save(std::shared_ptr<IStream> out, Handler hint);

	template<class T>
	struct isContainer : std::false_type {};

	template<class T>
	struct isContainer<Container<T>> : std::true_type {};

}; 
#endif
