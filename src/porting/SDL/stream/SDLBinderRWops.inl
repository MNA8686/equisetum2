#if !defined(_EQSTREAMIMPL_H_)
#define _EQSTREAMIMPL_H_

#include "SDL.h"
#include "stream/IStream.h"
#include <vector>
#include <memory>

namespace Equisetum2
{
	static Sint64 size(struct SDL_RWops *context)
	{
		auto stream = (IStream*)context->hidden.unknown.data1;
		return stream->Length();
	}

	static Sint64 seek(struct SDL_RWops *context, Sint64 offset, int whence)
	{
		SeekOrigin origin;

		switch (whence)
		{
		case RW_SEEK_SET:
			origin = SeekOrigin::Begin;
			break;
		case RW_SEEK_CUR:
			origin = SeekOrigin::Current;
			break;
		case RW_SEEK_END:
			origin = SeekOrigin::End;
			break;
		}

		auto stream = (IStream*)context->hidden.unknown.data1;
		return stream->Seek(offset, origin);
	}

	static size_t read(struct SDL_RWops *context, void *ptr, size_t size, size_t maxnum)
	{
		auto stream = (IStream*)context->hidden.unknown.data1;
		size_t count = 0;

		for (size_t i = 0; i < maxnum; i++)
		{
			auto opt = stream->Read(&((uint8_t*)ptr)[size * i], size);
			if (!opt)
			{
				break;
			}

			count++;
		}

		return count;
	}

	static size_t write(struct SDL_RWops *context, const void *ptr, size_t size, size_t num)
	{
		auto stream = (IStream*)context->hidden.unknown.data1;
		size_t count = 0;

		for (size_t i = 0; i < num; i++)
		{
			auto opt = stream->Write(&((const uint8_t*)ptr)[size * i], size);
			if (!opt)
			{
				break;
			}

			count++;
		}

		return count;
	}

	static int close(struct SDL_RWops *context)
	{
		// ストリームはスマートポインタで解放されるはず
		return 0;
	}

	// SDLのRWops丸投げクラス :p
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
					pRWops->close(pRWops);		// 中でSDL_FreeRWも呼ばれる
				}
			});
		}

		// ストリームからrwopsを生成する
		static SDL_RWops CreateFromStream(std::shared_ptr<IStream> stream)
		{
			SDL_RWops ops = {};

			ops.type = SDL_RWOPS_UNKNOWN;
			ops.hidden.unknown.data1 = stream.get();
			ops.size = size;
			ops.read = read;
			ops.write = write;
			ops.seek = seek;
			ops.close = close;

			return ops;
		}

	private:
		std::shared_ptr<SDL_RWops> m_pRWops;
	};
}

#endif
