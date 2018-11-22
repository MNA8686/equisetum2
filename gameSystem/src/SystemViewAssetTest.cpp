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
	auto menu = SystemWidgetMenu::Create(u8"メニュー");
	menu->SetPos({ 0.05f, 0.2f });
	m_vWidget.push_back(menu);

	auto pop = SystemWidgetPopView::Create(u8"戻る");
	menu->SetWidget(pop);

	auto textureTest = SystemWidgetPushView::Create(u8"テクスチャテスト", [this]()->std::shared_ptr<SystemView> {
		return SystemViewTextureTest::Create(u8"テクスチャテスト");
	});
	menu->SetWidget(textureTest);

	auto spriteTest = SystemWidgetPushView::Create(u8"スプライトテスト", [this]()->std::shared_ptr<SystemView> {
		return SystemViewSpriteTest::Create(u8"スプライトテスト");
	});
	menu->SetWidget(spriteTest);

	auto bgmTest = SystemWidgetPushView::Create(u8"BGMテスト", [this]()->std::shared_ptr<SystemView> {
		return SystemViewBGMTest::Create(u8"BGMテスト");
	});
	menu->SetWidget(bgmTest);

	auto seTest = SystemWidgetPushView::Create(u8"SEテスト", [this]()->std::shared_ptr<SystemView> {
		return SystemViewSETest::Create(u8"SEテスト");
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

