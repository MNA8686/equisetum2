#pragma once

#include "Equisetum2.h"
using namespace Equisetum2;

#include "SystemView.hpp"

class SystemWidgetSpin;
class SystemWidgetLabel;
class SystemViewBGMTest : public SystemView
{
public:
	SystemViewBGMTest() = default;
	virtual ~SystemViewBGMTest() = default;

	int Enter() override;
	int Do() override;
	int Render() override;

	static std::shared_ptr<SystemViewBGMTest> Create(const String & name);

protected:
	std::shared_ptr<BGM> m_bgm;
	std::shared_ptr<SystemWidgetLabel> m_labelPlay;
	int32_t m_volume = 100;
};

