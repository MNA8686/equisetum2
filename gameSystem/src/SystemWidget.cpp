#include "SystemWidget.hpp"

SystemWidget::SystemWidget()
{
}

SystemWidget::~SystemWidget()
{
}

std::shared_ptr<SystemWidgetNext> SystemWidgetNext::Create()
{
	return nullptr;
}

std::shared_ptr<SystemWidgetSpin> SystemWidgetSpin::Create(int32_t min, int32_t max, int32_t step, const std::function<void(int32_t)> cb)
{
	return nullptr;
}

void SystemWidgetSpin::SetRange(int32_t min, int32_t max)
{
	m_min = min;
	m_max = max;
	m_val = 0;
}

void SystemWidgetSpin::SetValue(int32_t val)
{
	m_val = val;
}

int SystemWidgetSpin::Do()
{
	// ��
	// Prev()
	// ��
	// Next
	// ��
	// �l�_�E��
	// �E
	// �l�A�b�v
}
