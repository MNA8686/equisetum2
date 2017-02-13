#if !defined(_EQSTREAMIMPL_H_)
#define _EQSTREAMIMPL_H_

#include "stream/IStream.h"
#include "SDL.h"
#include <vector>
#include <memory>

namespace Equisetum2
{
	// SDL‚ÌRWopsŠÛ“Š‚°ƒNƒ‰ƒX :p
	class SDLBinderRWops
	{
	public:

		inline int64_t Length() const
		{
			return SDL_RWsize(m_pRWops.get());
		}

		inline int64_t Position() const
		{
			return SDL_RWtell(m_pRWops.get());
		}

		inline size_t Read(uint8_t* pByteArray, size_t size)
		{
			return SDL_RWread(m_pRWops.get(), pByteArray, 1, size);
		}

		inline int ReadByte()
		{
			int byte = -1;

			uint8_t readByte;
			size_t readCount = Read(&readByte, 1);
			if (readCount == 1)
			{
				byte = readByte;
			}

			return byte;
		}

		inline size_t Write(const uint8_t* pByteArray, size_t size)
		{
			return SDL_RWwrite(m_pRWops.get(), pByteArray, 1, size);
		}

		inline int WriteByte(uint8_t writeByte)
		{
			return Write(&writeByte, 1);
		}

		inline int64_t Seek(int64_t offset, SeekOrigin origin)
		{
			int whence = 0;

			switch (origin)
			{
			case SeekOrigin::Begin:
				whence = RW_SEEK_SET;
				break;
			case SeekOrigin::Current:
				whence = RW_SEEK_CUR;
				break;
			case SeekOrigin::End:
				whence = RW_SEEK_END;
				break;
			default:
				return -1;
			}

			return SDL_RWseek(m_pRWops.get(), offset, whence);
		}

		SDLBinderRWops(SDL_RWops* pRWops)
		{
			m_pRWops = std::shared_ptr<SDL_RWops>(pRWops,
				[](SDL_RWops* pRWops){
				if (pRWops != nullptr)
				{
					pRWops->close(pRWops);		// ’†‚ÅSDL_FreeRW‚àŒÄ‚Î‚ê‚é
				}
			});
		}

	private:
		std::shared_ptr<SDL_RWops> m_pRWops;
	};
}

#endif
