#include "system/Logger.h"
#include "system/Exception.hpp"
#include "SystemViewBGMTest.hpp"
#include "SystemWidgetMenu.hpp"
#include "SystemWidgetSpin.hpp"
#include "SystemWidgetPopView.hpp"
#include "SystemWidgetPushView.hpp"
#include "SystemWidgetCustom.hpp"
#include "SystemWidgetChoice.hpp"
#include "Application.hpp"

std::shared_ptr<SystemViewBGMTest> SystemViewBGMTest::Create(const String & name)
{
	auto p = std::make_shared<SystemViewBGMTest>();

	p->m_name = name;
	p->Enter();

	return p;
}

int SystemViewBGMTest::Enter()
{
	auto menu = SystemWidgetMenu::Create(u8"メニュー");
	menu->SetPos({ 0.05f, 0.2f });
	m_vWidget.push_back(menu);

	auto pop = SystemWidgetPopView::Create(u8"戻る");
	menu->SetWidget(pop);

	auto choice = SystemWidgetChoice::Create(u8"ファイル選択", [this]()->std::vector<String> {
		std::vector<String> test;
		
		// BGMリストを取得する
		auto bgmList = Singleton<AssetManager>::GetInstance()->GetIdList("bgm");
		for (auto bgm : bgmList)
		{
			const String ext = Path::GetExtension(bgm);
			if (ext == ".ogg" ||
				ext == ".mp3" ||
				ext == ".wav")
			{
				const String nameWithoutExt = Path::GetFileNameWithoutExtension(bgm);
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
			m_bgm = nullptr;
			m_bgm = Singleton<AssetManager>::GetInstance()->Load<BGM>(item);
		}
	});
	menu->SetWidget(choice);

	auto play = SystemWidgetCustom::Create(u8"再生/停止", [this]()->bool {
		if (m_bgm)
		{
			if (m_bgm->IsPlaying())
			{
				m_bgm->Stop();
			}
			else
			{
				m_bgm->Play(false);
			}
		}
		return false;
	});
	menu->SetWidget(play);

	auto pause = SystemWidgetCustom::Create(u8"一時停止/再開", [this]()->bool {
		if (m_bgm)
		{
			if (m_bgm->IsPaused())
			{
				m_bgm->Resume();
			}
			else
			{
				m_bgm->Pause();
			}
		}
		return false;
	});
	menu->SetWidget(pause);

	auto pos = SystemWidgetSpin::Create(u8"ポジション設定", [this](int32_t val) {
	});
	pos->SetFormatCallBack([](int32_t val)->String {
		// "分:秒" のフォーマットで表示する
		return String::Sprintf("%d:%02d", val / 60, val % 60);
	});
	pos->OnEnter([this](int32_t val) {
		if (m_bgm)
		{
			if (!m_bgm->IsPlaying())
			{
				m_bgm->Play(false);
			}
			m_bgm->SetPos(static_cast<double>(val));
		}
	});
	// BGMの長さがわからないのでとりあえず10分までの範囲を設定可能とする
	pos->SetRange(0, (10 * 60) - 1, 1);
	pos->SetValue(0);
	menu->SetWidget(pos);

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

int SystemViewBGMTest::Do()
{
	String play;

	if (m_bgm)
	{
		m_bgm->SetVolume(m_volume / 100.f);
		
		
		if (m_bgm->IsPlaying())
		{
			if (m_bgm->IsPaused())
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

int SystemViewBGMTest::Render()
{
	if (m_bgm)
	{
		m_labelPlay->Render(this);
	}

	return 0;
}

