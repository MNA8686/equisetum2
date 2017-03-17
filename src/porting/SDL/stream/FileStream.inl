#include "stream/FileStream.h"

#include "stream/SDLBinderRWops.inl"

namespace Equisetum2
{
	class FileStream::StreamImpl : public SDLBinderRWops
	{
	public:
		StreamImpl(SDL_RWops* pRWops) : SDLBinderRWops(pRWops){}

		static std::shared_ptr<StreamImpl> OpenFromPath(const String& strPath, int openMethod)
		{
			std::shared_ptr<StreamImpl> impl;
			String strMethod;

			if (openMethod & (Method::Read | Method::Write | Method::Append))
			{
				if (openMethod & Method::Append)
				{
					strMethod += (openMethod & Method::Read) ? "ab+" : "ab";
				}
				else if ((openMethod & (Method::Read | Method::Write)) == (Method::Read | Method::Write))
				{
					strMethod += (openMethod & Method::Create) ? "wb+" : "rb+";
				}
				else if (openMethod & Method::Write)
				{
					strMethod += "wb";
				}
				else
				{
					strMethod += "rb";
				}

				if (auto pRWops = SDL_RWFromFile(strPath.c_str(), strMethod.c_str()))
				{
					impl = std::make_shared<StreamImpl>(pRWops);
				}
			}

			return impl;
		}
	};
}
