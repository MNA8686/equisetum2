#pragma once

#include "Equisetum2.h"
using namespace Equisetum2;

#include "SystemWidgetLabel.hpp"

class SystemWidgetPushView : public SystemWidget
{
public:
	SystemWidgetPushView() = default;
	~SystemWidgetPushView() = default;
	int Do(SystemView* pView) override;
	int Render(const SystemView* pView) override;
	Rect GetBox() const override;
	void SetPos(const PointF& pos) override;

	static std::shared_ptr<SystemWidgetPushView> Create(const String& label, const std::function<std::shared_ptr<SystemView>()>& cb);

private:
	std::shared_ptr<SystemWidgetLabel> m_label;
	std::function<std::shared_ptr<SystemView>()> m_cb;
};

