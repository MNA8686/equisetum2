#include "graphic/Image.hpp"
#include "system/Logger.h"
#include "graphic/ImageCompat.hpp"

#include "graphic/ImageCompatImpl.inl"

namespace Equisetum2
{
	ImageCompat::ImageCompat()
	{
		m_Impl = std::make_shared<ImageCompat::Impl>();
	}

	bool ImageCompat::InitFromStream(std::shared_ptr<IStream>& stream)
	{
		return m_Impl->InitFromStream(stream);
	}

	bool ImageCompat::SaveToStream(std::shared_ptr<IStream> stream)
	{
		return m_Impl->SaveToStream(stream);
	}

	bool ImageCompat::Resize(uint32_t width, uint32_t height)
	{
		return m_Impl->Resize(width, height);
	}

	uint32_t ImageCompat::Width()
	{
		return m_Impl->Width();
	}

	uint32_t ImageCompat::Height()
	{
		return m_Impl->Height();
	}

	uint32_t ImageCompat::Pitch()
	{
		return m_Impl->Pitch();
	}

	Color* ImageCompat::Data()
	{
		return m_Impl->Data();
	}
}

