#pragma once

#include "Equisetum2.h"
using namespace Equisetum2;

#include "SystemView.hpp"
#include "SystemWidgetLabel.hpp"

class SystemChoiceDialog
{
public:
	static std::shared_ptr<SystemChoiceDialog> Create(const String& title, const std::vector<String>& vItem, const std::function<void(int index, const String& item)> cb);

	SystemChoiceDialog() = default;
	virtual ~SystemChoiceDialog() = default;

	int Do(SystemView* pView);
	int Render(const SystemView* pView);
	bool IsActive() const;

protected:
	std::function<void(int index, const String& item)> m_cb;
	std::vector<String> m_vItem;
	String m_title;
	bool m_isActive = false;
	Rect m_rectBorder;			// ŠO˜g
	Rect m_rectInnerFrame;		// “à˜g

	std::shared_ptr<RectRenderer> m_rectRendererBorder;
	std::shared_ptr<RectRenderer> m_rectRendererInnerFrame;
	std::shared_ptr<SystemWidgetLabel> m_label;
};


