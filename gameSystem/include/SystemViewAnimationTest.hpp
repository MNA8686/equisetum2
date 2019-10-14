#pragma once

#include "Equisetum2.h"
using namespace Equisetum2;

#include "SystemView.hpp"

class SystemWidgetSpin;
class SystemViewAnimationTest : public SystemView
{
public:
	SystemViewAnimationTest() = default;
	virtual ~SystemViewAnimationTest() = default;

	int Enter() override;
	int Do() override;
	int Render() override;

	static std::shared_ptr<SystemViewAnimationTest> Create(const String & name);

protected:
	void LoadAnimation();
	int32_t m_rate = 100;
	int32_t m_animationTagIndex = -1;	// ñ¢ê›íËéû -1
	int32_t m_animationPtr = 0;
	int32_t m_frame = 0;
	int32_t m_degree = 0;
	bool m_playing = false;
	Point m_spritePos;
	std::shared_ptr<SpriteRenderer> m_spriteRenderer;
	std::shared_ptr<Animation> m_animation;
	std::shared_ptr<SystemWidgetSpin> m_spinTag;
	std::shared_ptr<SystemWidgetSpin> m_spinAnim;
	String m_item;
	Color m_color{128, 128, 128, 128};
};

