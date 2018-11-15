#pragma once

#include "Equisetum2.h"
using namespace Equisetum2;

#include "SystemView.hpp"

class SystemWidgetLabel;
class SystemWidgetSpin;
class SystemViewSETest : public SystemView
{
public:
	SystemViewSETest() = default;
	virtual ~SystemViewSETest() = default;

	int Enter() override;
	int Do() override;
	int Render() override;

	static std::shared_ptr<SystemViewSETest> Create(const String & name);

protected:
	std::shared_ptr<SystemWidgetLabel> m_labelPlay;
	std::shared_ptr<SE> m_se;
	int32_t m_volume = 100;
};

