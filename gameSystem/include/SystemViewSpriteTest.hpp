#pragma once

#include "Equisetum2.h"
using namespace Equisetum2;

#include "SystemView.hpp"

class SystemWidgetSpin;
class SystemViewSpriteTest : public SystemView
{
public:
	SystemViewSpriteTest() = default;
	virtual ~SystemViewSpriteTest() = default;

	int Enter() override;
	int Do() override;
	int Render() override;

	static std::shared_ptr<SystemViewSpriteTest> Create(const String & name);

protected:
	void LoadSprite();
	int32_t m_rate = 100;
	int32_t m_ptr = 0;
	Point m_spritePos;
	std::shared_ptr<SpriteRenderer> m_spriteRenderer;
	std::shared_ptr<SystemWidgetSpin> m_spinAnim;
	String m_item;
	Color m_color{128, 128, 128, 128};
};

