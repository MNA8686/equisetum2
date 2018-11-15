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
	auto menu = SystemWidgetMenu::Create(u8"���j���[");
	menu->SetPos({ 0.05f, 0.2f });
	m_vWidget.push_back(menu);

	auto pop = SystemWidgetPopView::Create(u8"�߂�");
	menu->SetWidget(pop);

	auto choice = SystemWidgetChoice::Create(u8"�t�@�C���I��", [this]()->std::vector<String> {
		std::vector<String> test;
		
		// BGM���X�g���擾����
		auto bgmList = Directory::GetFiles(Path::GetFullPath("bgm"));
		if (bgmList)
		{
			for (auto bgm : *bgmList)
			{
				test.push_back(Path::GetFileNameWithoutExtension(bgm));
			}
		}
		
		return test;
	}, [this](int32_t index, const String& item) {
		if (index >= 0)
		{
			m_bgm = Singleton<AssetManager>::GetInstance()->Load<BGM>(item);
		}
	});
	menu->SetWidget(choice);

	auto play = SystemWidgetCustom::Create(u8"�Đ�/��~", [this]()->bool {
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

	auto pause = SystemWidgetCustom::Create(u8"�ꎞ��~/�ĊJ", [this]()->bool {
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

	auto vol = SystemWidgetSpin::Create(u8"�{�����[��", [this](int32_t val) {
		m_volume = val;
	});
	vol->SetRange(0, 100, 1);
	vol->SetValue(100);
	menu->SetWidget(vol);

	menu->SetFocus(true);

	return 0;
}

int SystemViewBGMTest::Do()
{
	if (m_bgm)
	{
		m_bgm->SetVolume(m_volume / 100.f);
	}
	return 0;
}

int SystemViewBGMTest::Render()
{
	return 0;
}

