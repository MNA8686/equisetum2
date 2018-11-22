#include "system/Logger.h"
#include "system/Exception.hpp"
#include "SystemViewTopMenu.hpp"
#include "SystemViewAssetTest.hpp"
#include "SystemWidgetMenu.hpp"
#include "SystemWidgetSpin.hpp"
#include "SystemWidgetPopView.hpp"
#include "SystemWidgetPushView.hpp"
#include "Application.hpp"

std::shared_ptr<TopMenu> TopMenu::Create(const String & name)
{
	auto p = std::make_shared<TopMenu>();

	p->m_name = name;
	p->Enter();

	return p;
}

int TopMenu::Enter()
{
	auto menu = SystemWidgetMenu::Create(u8"メニュー");
	menu->SetPos({ 0.05f, 0.2f });
	m_vWidget.push_back(menu);

	auto returnToGame = SystemWidgetPopView::Create(u8"ゲームへ戻る");
	menu->SetWidget(returnToGame);

	auto inputTest = SystemWidgetPushView::Create(u8"入力テスト", [this]()->std::shared_ptr<SystemView> {
		return{};
	});
	menu->SetWidget(inputTest);

	auto assetTest = SystemWidgetPushView::Create(u8"アセットテスト", [this]()->std::shared_ptr<SystemView> {
		return AssetTest::Create(u8"アセットテスト");
	});
	menu->SetWidget(assetTest);

	menu->SetFocus(true);

	return 0;
}

int TopMenu::Do()
{
	return 0;
}

