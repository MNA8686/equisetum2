#pragma once

#include "Equisetum2.h"
using namespace Equisetum2;

#include "SystemView.hpp"

class AssetTest : public SystemView
{
public:
	AssetTest() = default;
	virtual ~AssetTest() = default;

	int Enter() override;
	int Do() override;
	int Render() override;

	static std::shared_ptr<AssetTest> Create(const String & name);

protected:
	int32_t m_rate = 100;
	int32_t m_ptr = 0;
	Point m_spritePos;
	std::shared_ptr<SpriteRenderer> m_spriteRenderer;
};

