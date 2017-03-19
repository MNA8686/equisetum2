#include "stream/MemoryStream.h"

#include "stream/SDLBinderRWops.inl"

namespace Equisetum2
{
	class MemoryStream::StreamImpl : public SDLBinderRWops
	{
	public:
		StreamImpl(SDL_RWops* pRWops) : SDLBinderRWops(pRWops){}

		static std::shared_ptr<StreamImpl> OpenFromBuf(const void* buf, int size)
		{
			std::shared_ptr<StreamImpl> impl;

			if(buf && size > 0)
			{
				if (auto pRWops = SDL_RWFromConstMem(buf, size))
				{
					impl = std::make_shared<StreamImpl>(pRWops);
				}
			}

			return impl;
		}

		static std::shared_ptr<StreamImpl> OpenFromBuf(void* buf, int size, bool writable=true)
		{
			std::shared_ptr<StreamImpl> impl;

			if (buf && size > 0)
			{
				if (writable)
				{
					if (auto pRWops = SDL_RWFromMem(buf, size))
					{
						impl = std::make_shared<StreamImpl>(pRWops);
					}
				}
				else
				{
					impl = OpenFromBuf((const void*)buf, size);
				}
			}

			return impl;
		}
	};
}
