#include "HitBox.hpp"

bool HitBox::SetCurrectRects(int32_t current)
{
	if (current < m_vManager.Size())
	{
		// マイナスを指定された場合は判定なしと見なされる
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

//-------------------------------------------------------------------------------

HitBoxIterator::iterator HitBoxIterator::begin() const
{
	return iterator(m_pos, m_pHitBox->begin());
}

HitBoxIterator::iterator HitBoxIterator::end() const
{
	return iterator(m_pos, m_pHitBox->end());
}

HitBoxIterator::HitBoxIterator(const Point_t<FixedDec>& pos, const HitBox* pHitBox)
{
	m_pos = pos;
	m_pHitBox = pHitBox;
}

bool HitBoxIterator::HitTest(const Rect_t<FixedDec>& rect) const
{
	for (auto& selfRect : *this)
	{
		if (CollisionDetection::HitTest(rect, selfRect))
		{
			return true;
		}
	}

	return false;
}

HitBoxIterator::iterator::iterator(const Point_t<FixedDec>& pos, const HitBox::const_iterator& it)
{
	m_pos = pos;
	m_it = it;
}

void HitBoxIterator::iterator::calc()
{
	m_rect.x = m_pos.x + m_it->x;
	m_rect.y = m_pos.y + m_it->y;
	m_rect.width = m_it->width;
	m_rect.height = m_it->height;
}

HitBoxIterator::iterator& HitBoxIterator::iterator::operator++()
{
	m_it++;
	return *this;
}

HitBoxIterator::iterator HitBoxIterator::iterator::operator++(int)
{
	auto it = *this;

	++(*this);

	return it;
}

const Rect_t<FixedDec>& HitBoxIterator::iterator::operator*()
{
	calc();
	return m_rect;
}

const Rect_t<FixedDec>* HitBoxIterator::iterator::operator->()
{
	calc();
	return &m_rect;
}

bool HitBoxIterator::iterator::operator== (const iterator& r) const
{
	return m_it == r.m_it;
}

bool HitBoxIterator::iterator::operator!= (const iterator& r) const
{
	return !(m_it == r.m_it);
}

