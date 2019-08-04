#include "system/Logger.h"
#include "system/Exception.hpp"
#include "SystemViewSETest.hpp"
#include "SystemWidgetMenu.hpp"
#include "SystemWidgetSpin.hpp"
#include "SystemWidgetPopView.hpp"
#include "SystemWidgetPushView.hpp"
#include "SystemWidgetCustom.hpp"
#include "SystemWidgetChoice.hpp"
#include "Application.hpp"

std::shared_ptr<SystemViewSETest> SystemViewSETest::Create(const String & name)
{
	auto p = std::make_shared<SystemViewSETest>();

	p->m_name = name;
	p->Enter();

	return p;
}

int SystemViewSETest::Enter()
{
	auto menu = SystemWidgetMenu::Create(u8"メニュー");
	menu->SetPos({ 0.05f, 0.2f });
	m_vWidget.push_back(menu);

	auto pop = SystemWidgetPopView::Create(u8"戻る");
	menu->SetWidget(pop);

	auto choice = SystemWidgetChoice::Create(u8"ファイル選択", [this]()->std::vector<String> {
		std::vector<String> test;
		
		// SEリストを取得する
		auto seList = Singleton<AssetManager>::GetInstance()->GetIdList("se");
		for (auto se : seList)
		{
			const String ext = Path::GetExtension(se);
			if (ext == ".ogg" ||
				ext == ".mp3" ||
				ext == ".wav")
			{
				const String nameWithoutExt = Path::ChangeExtension(se, "");
				auto result = std::find(test.begin(), test.end(), nameWithoutExt);
				if (result == test.end())
				{
					test.push_back(nameWithoutExt);
				}
			}
		}
		
		return test;
	}, [this](int32_t index, const String& item) {
		if (index >= 0)
		{
			m_se = nullptr;
			m_se = Singleton<AssetManager>::GetInstance()->Load<SE>(item);
		}
	});
	menu->SetWidget(choice);

	auto play = SystemWidgetCustom::Create(u8"再生/停止", [this]()->bool {
		if (m_se)
		{
			if (m_se->IsPlaying())
			{
				m_se->Stop();
			}
			else
			{
				m_se->Play(false);
			}
		}
		return false;
	});
	menu->SetWidget(play);

	auto pause = SystemWidgetCustom::Create(u8"一時停止/再開", [this]()->bool {
		if (m_se)
		{
			if (m_se->IsPaused())
			{
				m_se->Resume();
			}
			else
			{
				m_se->Pause();
			}
		}
		return false;
	});
	menu->SetWidget(pause);

	auto vol = SystemWidgetSpin::Create(u8"ボリューム", [this](int32_t val) {
		m_volume = val;
	});
	vol->SetRange(0, 100, 1);
	vol->SetValue(100);
	menu->SetWidget(vol);

	menu->SetFocus(true);

	m_labelPlay = SystemWidgetLabel::Create(u8"▶■|");
	m_labelPlay->SetPos({0.5f, 0.9f});

	return 0;
}

int SystemViewSETest::Do()
{
	String play;

	if (m_se)
	{
		m_se->SetVolume(m_volume / 100.f);
		
		
		if (m_se->IsPlaying())
		{
			if (m_se->IsPaused())
			{
				play = u8"||";
			}
			else
			{
				play = u8"▶";
			}
		}
		else
		{
			play = u8"■";
		}
	}

	m_labelPlay->SetText(play);

	return 0;
}

int SystemViewSETest::Render()
{
	if (m_se)
	{
		m_labelPlay->Render(this);
	}

	return 0;
}

