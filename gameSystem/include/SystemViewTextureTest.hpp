#pragma once

#include "Equisetum2.h"
using namespace Equisetum2;

#include "SystemView.hpp"

class SystemViewTextureTest : public SystemView
{
public:
	SystemViewTextureTest() = default;
	virtual ~SystemViewTextureTest() = default;

	int Enter() override;
	int Do() override;
	int Render() override;

	static std::shared_ptr<SystemViewTextureTest> Create(const String & name);

protected:
	int32_t m_rate = 100;
	Point m_spritePos;
	std::shared_ptr<SpriteRenderer> m_spriteRenderer;
	Color m_color{128, 128, 128, 128};
};

