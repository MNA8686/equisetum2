#include "system/Logger.h"
#include "input/TouchCompat.h"

#include "input/TouchCompatImpl.inl"

namespace Equisetum2
{
	TouchCompat::TouchCompat()
	{
		m_Impl = std::make_shared<TouchCompat::Impl>();
	}

	void TouchCompat::Prepare()
	{
		m_Impl->Prepare();
	}

	void TouchCompat::Update()
	{
		m_Impl->Update();
	}

	const Point<float>& TouchCompat::Pos(int fingerIndex) const
	{
		return m_Impl->Pos(fingerIndex);
	}

	const Point<float>& TouchCompat::Delta(int fingerIndex) const
	{
		return m_Impl->Delta(fingerIndex);
	}

	const stKeyState& TouchCompat::Button(int fingerIndex) const
	{
		return m_Impl->Button(fingerIndex);
	}

	int64_t TouchCompat::FingerId(int fingerIndex) const
	{
		return m_Impl->FingerId(fingerIndex);
	}

	int TouchCompat::NumSupportedFinger() const
	{
		return m_Impl->NumSupportedFinger();
	}
}
