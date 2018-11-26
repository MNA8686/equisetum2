#pragma once

#include "Equisetum2.h"
using namespace Equisetum2;

#include "SystemView.hpp"

class SystemWidgetLabel;
class SystemViewFontTest : public SystemView
{
public:
	SystemViewFontTest() = default;
	virtual ~SystemViewFontTest() = default;

	int Enter() override;
	int Do() override;
	int Render() override;

	static std::shared_ptr<SystemViewFontTest> Create(const String & name);

protected:
	void LoadFont();
	int32_t m_fontSize = 16;
	String m_item;

	Color m_color{128, 128, 128, 128};
	PointF m_textPos{0.05f, 0.7f};
	std::shared_ptr<SystemWidgetLabel> m_label;
};

