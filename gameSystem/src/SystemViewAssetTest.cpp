#include "system/Logger.h"
#include "system/Exception.hpp"
#include "SystemViewAssetTest.hpp"
#include "SystemViewTextureTest.hpp"
#include "SystemViewSpriteTest.hpp"
#include "SystemViewBGMTest.hpp"
#include "SystemViewSETest.hpp"
#include "SystemWidgetMenu.hpp"
#include "SystemWidgetSpin.hpp"
#include "SystemWidgetPopView.hpp"
#include "SystemWidgetPushView.hpp"
#include "SystemWidgetCustom.hpp"
#include "SystemWidgetChoice.hpp"
#include "Application.hpp"

std::shared_ptr<AssetTest> AssetTest::Create(const String & name)
{
	auto p = std::make_shared<AssetTest>();

	p->m_name = name;
	p->Enter();

	return p;
}

int AssetTest::Enter()
{
	auto menu = SystemWidgetMenu::Create(u8"���j���[");
	menu->SetPos({ 0.05f, 0.2f });
	m_vWidget.push_back(menu);

	auto pop = SystemWidgetPopView::Create(u8"�߂�");
	menu->SetWidget(pop);

	auto textureTest = SystemWidgetPushView::Create(u8"�e�N�X�`���e�X�g", [this]()->std::shared_ptr<SystemView> {
		return SystemViewTextureTest::Create(u8"�e�N�X�`���e�X�g");
	});
	menu->SetWidget(textureTest);

	auto spriteTest = SystemWidgetPushView::Create(u8"�X�v���C�g�e�X�g", [this]()->std::shared_ptr<SystemView> {
		return SystemViewSpriteTest::Create(u8"�X�v���C�g�e�X�g");
	});
	menu->SetWidget(spriteTest);

	auto bgmTest = SystemWidgetPushView::Create(u8"BGM�e�X�g", [this]()->std::shared_ptr<SystemView> {
		return SystemViewBGMTest::Create(u8"BGM�e�X�g");
	});
	menu->SetWidget(bgmTest);

	auto seTest = SystemWidgetPushView::Create(u8"SE�e�X�g", [this]()->std::shared_ptr<SystemView> {
		return SystemViewSETest::Create(u8"SE�e�X�g");
	});
	menu->SetWidget(seTest);

	menu->SetFocus(true);

	return 0;
}

int AssetTest::Do()
{
	return 0;
}

int AssetTest::Render()
{
	return 0;
}

