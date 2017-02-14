#include "system/Logger.h"
#include "SDL.h"

namespace Equisetum2
{
	class SDLTimer
	{
	public:
		SDLTimer()
		{
			if (SDL_WasInit(SDL_INIT_TIMER) == 0)
			{
				SDL_InitSubSystem(SDL_INIT_TIMER);
			}
		}

		virtual ~SDLTimer()
		{
			SDL_QuitSubSystem(SDL_INIT_TIMER);
		}
	};

	class SystemTimerCompat::Impl final : public SDLTimer
	{
	public:
		Impl(){}

		uint32_t Ticks() const
		{
			return SDL_GetTicks();
		}
	};
}
