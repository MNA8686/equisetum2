#include "HitBox.hpp"

bool HitBox::SetCurrectRects(int32_t current)
{
	if (current >= 0 && current < m_vManager.Size())
	{
		m_current = current;
		return true;
	}

	return false;
}

void HitBox::Push(const Rect_t<FixedDec>& rect)
{
	m_vRect.PushBack(rect);
}

bool HitBox::Packaging()
{
	if (m_vManager.Size() == 0)
	{
		int32_t size = m_vRect.Size();
		if (size > 0)
		{
			Manager mgr;
			mgr.begin = 0;
			mgr.size = size;
			m_vManager.PushBack(mgr);
			return true;
		}
	}
	else
	{
		Manager mgrLast = m_vManager.Back();
		if (mgrLast.begin + mgrLast.size != m_vRect.Size())
		{
			Manager mgr;
			mgr.begin = mgrLast.begin + mgrLast.size;
			mgr.size = m_vRect.Size() - mgr.begin;
			m_vManager.PushBack(mgr);
			return true;
		}
	}
	
	return false;
}

HitBox::iterator HitBox::begin()
{
	if (m_current < 0)
	{
		return nullptr;
	}

	int32_t index = m_vManager[m_current].begin;
	return &m_vRect[index];
}

HitBox::iterator HitBox::end()
{
	if (m_current < 0)
	{
		return nullptr;
	}

	int32_t index = m_vManager[m_current].begin + m_vManager[m_current].size;
	return &m_vRect[index];
}

HitBox::const_iterator HitBox::begin() const
{
	if (m_current < 0)
	{
		return nullptr;
	}

	int32_t index = m_vManager[m_current].begin;
	return &m_vRect[index];
}

HitBox::const_iterator HitBox::end() const
{
	if (m_current < 0)
	{
		return nullptr;
	}

	int32_t index = m_vManager[m_current].begin + m_vManager[m_current].size;
	return &m_vRect[index];
}


