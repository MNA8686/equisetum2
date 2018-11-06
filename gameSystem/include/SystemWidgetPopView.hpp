#pragma once

#include "Equisetum2.h"
using namespace Equisetum2;

#include "SystemWidgetLabel.hpp"

class SystemWidgetPopView : public SystemWidget
{
public:
	SystemWidgetPopView() = default;
	~SystemWidgetPopView() = default;
	int Do(SystemView* pView) override;
	int Render(const SystemView* pView) override;

	static std::shared_ptr<SystemWidgetPopView> Create(const String& label);
	Rect GetBox() const override;
	void SetPos(const PointF& pos) override;

private:
	std::shared_ptr<SystemWidgetLabel> m_label;
};

