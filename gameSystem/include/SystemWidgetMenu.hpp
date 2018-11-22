#pragma once

#include "Equisetum2.h"
using namespace Equisetum2;

#include "SystemWidget.hpp"

class SystemWidgetMenu : public SystemWidget
{
public:
	SystemWidgetMenu () = default;
	virtual ~SystemWidgetMenu() = default;

	int SetWidget(std::shared_ptr<SystemWidget> pWidget);
	virtual int Do(SystemView* pView) override;
	virtual int Render(const SystemView* pView) override;

	static std::shared_ptr<SystemWidgetMenu> Create(const String& label);
	Rect GetBox() const override;

protected:
	std::vector<std::shared_ptr<SystemWidget>> m_vWidget;
	int m_focus = 0;
	int m_cursolMoveCounter = 0;
	std::shared_ptr<RectRenderer> m_rectRenderer;
	Rect m_cursorNow;
	Rect m_cursorSrc;
	Rect m_cursorDest;
};
