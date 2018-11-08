#include "SystemWidget.hpp"
#include "Application.hpp"

SystemWidget::SystemWidget()
{
}

SystemWidget::~SystemWidget()
{
}

void SystemWidget::Prev()
{
	m_stat = Stat::Prev;
}

void SystemWidget::Next()
{
	m_stat = Stat::Next;
}

void SystemWidget::SetFocus(bool focus)
{
	m_focus = focus;
}

bool SystemWidget::GetFocus() const
{
	return m_focus;
}

SystemWidget::Stat SystemWidget::GetStat() const
{
	return m_stat;
}

void SystemWidget::Prepare()
{
	m_stat = Stat::Idle;
}

void SystemWidget::SetPos(const PointF& pos)
{
	m_pos = pos;
}

PointF SystemWidget::GetPos() const
{
	return m_pos;
}

void SystemWidget::SetEnable(bool enable)
{
	m_enable = enable;
}

void SystemWidget::Setlayer(int layer)
{
	m_layer = layer;
}


