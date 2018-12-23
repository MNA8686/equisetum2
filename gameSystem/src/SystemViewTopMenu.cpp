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
	if (p->Enter() != 0)
	{
		p = nullptr;
	}

	return p;
}

int TopMenu::Enter()
{
	int ret = -1;

	EQ_DURING
	{
		auto menu = SystemWidgetMenu::Create(u8"メニュー");
		if (!menu)
		{
			EQ_THROW("");
		}
		menu->SetPos({ 0.05f, 0.2f });
		m_vWidget.push_back(menu);

		auto returnToGame = SystemWidgetPopView::Create(u8"ゲームへ戻る");
		if (!returnToGame)
		{
			EQ_THROW("");
		}
		menu->SetWidget(returnToGame);

		auto inputTest = SystemWidgetPushView::Create(u8"入力テスト", [this]()->std::shared_ptr<SystemView> {
			return{};
		});
		if (!inputTest)
		{
			EQ_THROW("");
		}
		menu->SetWidget(inputTest);

		auto assetTest = SystemWidgetPushView::Create(u8"アセットテスト", [this]()->std::shared_ptr<SystemView> {
			return AssetTest::Create(u8"アセットテスト");
		});
		if (!assetTest)
		{
			EQ_THROW("");
		}
		menu->SetWidget(assetTest);

		menu->SetFocus(true);

		ret = 0;
	}
	EQ_HANDLER
	{
		Logger::OutputError(EQ_GET_HANDLER().what());
	}
	EQ_END_HANDLER
		
	return ret;
}

int TopMenu::Do()
{
	return 0;
}

