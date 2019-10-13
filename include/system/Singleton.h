#if !defined(_EQSINGLETON_H_)
#define _EQSINGLETON_H_

#include <mutex>

namespace Equisetum2
{
	class SingletonFinalizer final
	{
	public:
		typedef void(*FinalizerFunc)();

		static void AddFinalizer(FinalizerFunc func);
		static void Finalize();
	};

	template <typename T>
	class Singleton final
	{
	public:

		static T* GetInstance()
		{
//			std::call_once(m_InitFlag, Create);
			//			assert(instance);

			if (!m_InitFlag)
			{
				Create();
				m_InitFlag = true;
			}

			return m_Instance;
		}

	private:

		static void Create()
		{
			m_Instance = new T;
			SingletonFinalizer::AddFinalizer(&Singleton<T>::Destroy);
		}

		static void Destroy()
		{
			delete m_Instance;
			m_Instance = nullptr;
		}

//		static std::once_flag m_InitFlag;
		static bool m_InitFlag;
		static T* m_Instance;
	};

//	template <typename T> std::once_flag Singleton<T>::m_InitFlag;
	template <typename T> bool Singleton<T>::m_InitFlag = false;
	template <typename T> T* Singleton<T>::m_Instance = nullptr;
}

#endif
