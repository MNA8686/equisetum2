#pragma once

#include "Equisetum2.h"
using namespace Equisetum2;

#include "SystemWidgetLabel.hpp"

class SystemWidgetCustom : public SystemWidget
{
public:
	SystemWidgetCustom() = default;
	~SystemWidgetCustom() = default;
	int Do(SystemView* pView) override;
	int Render(const SystemView* pView) override;
	Rect GetBox() const override;
	void SetPos(const PointF& pos) override;
	
	static std::shared_ptr<SystemWidgetCustom> Create(const String& label, const std::function<bool (void)> cb);

private:
	std::function<bool(void)> m_cb;
	bool m_exclusive = false;
	std::shared_ptr<SystemWidgetLabel> m_label;
};

