#pragma once

#include "Equisetum2.h"
using namespace Equisetum2;

#include "SystemWidgetLabel.hpp"

class SystemChoiceDialog;
class SystemWidgetChoice : public SystemWidget
{
public:
	SystemWidgetChoice() = default;
	~SystemWidgetChoice() = default;
	int Do(SystemView* pView) override;
	int Render(const SystemView* pView) override;
	Rect GetBox() const override;
	void SetPos(const PointF& pos) override;
	
	static std::shared_ptr<SystemWidgetChoice> Create(const String& label, const std::function<std::vector<String>()> cb);

private:
	std::shared_ptr<SystemWidgetLabel> m_label;
	std::vector<String> m_vItem;
	std::function<std::vector<String>()> m_cb;
	bool m_exclusive = false;
	std::shared_ptr<SystemChoiceDialog> m_dialog;
};

