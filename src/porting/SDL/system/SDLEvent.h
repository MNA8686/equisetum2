#if !defined(_EQSDLEVENT_H_)
#define _EQSDLEVENT_H_

#include "SDL.h"
#include <unordered_map>
#include <functional>

namespace Equisetum2
{
	// SDLのイベントマネージャークラス(シングルトン)
	class SDLEventManager final
	{
	public:
		friend Singleton<SDLEventManager>;	// シングルトンからインスタンスを作成してもらえるようにする

		void AddListener(void* key, std::function<void(SDL_Event e, void* key)>& cb)
		{
			m_map[key] = cb;
		}

		void RemoveListener(void* key)
		{
			m_map.erase(key);
		}

		void Dispatch(SDL_Event e)
		{
			for (auto cb : m_map)
			{
				cb.second(e, cb.first);
			}
		}

	private:

		SDLEventManager(){}	// インスタンス作成封じ

		std::unordered_map<void*, std::function<void(SDL_Event e, void* key)> > m_map;
	};

	// SDLのイベントリスナークラス
	class SDLEventListener final
	{
	public:

		SDLEventListener(void* key, std::function<void(SDL_Event e, void* key)> cb)
		{
			m_Key = key;

			if (m_Key != 0)
			{
				Singleton<SDLEventManager>::GetInstance()->AddListener(m_Key, cb);
			}
		}

		~SDLEventListener()
		{
			if (m_Key != 0)
			{
				Singleton<SDLEventManager>::GetInstance()->RemoveListener(m_Key);
			}
		}

	private:

		void* m_Key = 0;
	};
}

#endif
