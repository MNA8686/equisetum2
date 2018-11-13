#pragma once

#include "Equisetum2.h"
using namespace Equisetum2;

#include "SystemView.hpp"

class TopMenu : public SystemView
{
public:
	TopMenu() = default;
	virtual ~TopMenu() = default;

	int Enter() override;
	int Do() override;

	static std::shared_ptr<TopMenu> Create(const String& name);

protected:
	std::shared_ptr<SystemWidget> m_returnToGame;
	std::shared_ptr<SystemWidget> m_inputTest;
	std::shared_ptr<SystemWidget> m_assetTest;
};

