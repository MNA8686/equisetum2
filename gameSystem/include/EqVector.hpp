#if !defined(_EQVECTOR_H_)
#define _EQVECTOR_H_

#include "Equisetum2.h"
using namespace Equisetum2;

#include "EqHeap.hpp"

template<typename T>
class EqVector final
{
public:
	using iterator = T*;
	using const_iterator = const T*;

 	EqVector() = default;
	EqVector(int32_t num)
	{
		Resize(num);
	}
	
	EqVector(const EqVector<T>& src)
	{
		*this = src;
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

		if (m_reservedSize > 0)
		{
			Clear();
			heap->Delete(m_handler);
			m_reservedSize = 0;
			m_handler = 0;
		}

		if (src.m_reservedSize > 0)
		{
			EqHeap::Handler handler = heap->New(src.m_reservedSize * sizeof(T));
			if (handler != 0)
			{
				if (std::is_class<T>::value)
				{
					//コピーコンストラクタ呼び出し
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

	T& operator[](std::size_t index)
	{
		auto heap = Singleton<EqHeap>::GetInstance();
		return heap->Ref<T>(m_handler)[index];
	}

	const T& operator[](std::size_t index) const
	{
		auto heap = Singleton<EqHeap>::GetInstance();
		return heap->Ref<T>(m_handler)[index];
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
					//コピーコンストラクタ呼び出し
					CallCopyConstructor(reinterpret_cast<T*>(heap->Ref(handler)), Data(), m_usedSize);

					// 古い方の配列に入っているオブジェクトのデストラクタを呼ぶ
					CallDestructor(Data(), m_usedSize);
				}
				else
				{
					memcpy(heap->Ref(handler), Data(), m_usedSize * sizeof(T));
				}

				m_reservedSize = reserveSize;

				if (m_handler != 0)
				{
					// 新しく確保したメモリと古いメモリを入れ替える
					heap->Swap(m_handler, handler);
					// 古いほうのメモリを削除
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
					// 小さくする場合ははじき出される部分だけデストラクタを呼ぶ
					CallDestructor(&Data()[size], m_usedSize - size);
				}

				// 未使用になった領域をゼロ埋め
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
					// 大きくなった分だけコンストラクタを呼ぶ
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

	iterator Erase(iterator pos)
	{
		int32_t index = 0;
		auto data = Data();

		for (int32_t i = 0; i < m_usedSize; i++)
		{
			if (pos == &data[i])
			{
				if (std::is_class<T>::value)
				{
					// 古い方の配列に入っているオブジェクトのデストラクタを呼ぶ
					CallDestructor(&data[i], 1);
				}
				memset(&data[i], 0, sizeof(T));
				break;
			}
			index++;
		}

		for (int32_t i = index + 1; i < m_usedSize; i++)
		{
			if (std::is_class<T>::value)
			{
				//コピーコンストラクタ呼び出し
				CallCopyConstructor(&data[i-1], &data[i], 1);

				// 古い方のオブジェクトのデストラクタを呼ぶ
				CallDestructor(&data[i], m_usedSize);
			}
			else
			{
				memcpy(&data[i-1], &data[i], sizeof(T));
			}
			memset(&data[i], 0, sizeof(T));
		}

		if (index != m_usedSize)
		{
			m_usedSize--;
			return pos;
		}
		else
		{
			return end();
		}
	}

	void PushBack(const T& src)
	{
		// まずはメモリだけ確保する
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

	T& Back()
	{
		return Data()[m_usedSize - 1];
	}

	int32_t Size() const
	{
		return m_usedSize;
	}

	const_iterator begin() const
	{
		return Data();
	}

	const_iterator end() const
	{
		return Data() + m_usedSize;
	}

	iterator begin()
	{
		return Data();
	}

	iterator end()
	{
		return Data() + m_usedSize;
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
