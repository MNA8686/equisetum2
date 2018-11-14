#pragma once

#include "Equisetum2.h"
using namespace Equisetum2;

#include "SystemView.hpp"

class SystemWidgetSpin;
class SpriteTest : public SystemView
{
public:
	SpriteTest() = default;
	virtual ~SpriteTest() = default;

	int Enter() override;
	int Do() override;
	int Render() override;

	static std::shared_ptr<SpriteTest> Create(const String & name);

protected:
	int32_t m_rate = 100;
	int32_t m_ptr = 0;
	Point m_spritePos;
	std::shared_ptr<SpriteRenderer> m_spriteRenderer;
	std::shared_ptr<SystemWidgetSpin> m_spinAnim;
};

