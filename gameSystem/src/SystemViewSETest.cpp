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
	auto menu = SystemWidgetMenu::Create(u8"���j���[");
	menu->SetPos({ 0.05f, 0.2f });
	m_vWidget.push_back(menu);

	auto pop = SystemWidgetPopView::Create(u8"�߂�");
	menu->SetWidget(pop);

	auto choice = SystemWidgetChoice::Create(u8"�t�@�C���I��", [this]()->std::vector<String> {
		std::vector<String> test;
		
		// SE���X�g���擾����
		auto seList = Directory::GetFiles(Path::GetFullPath("se"));
		if (seList)
		{
			for (auto se : *seList)
			{
				test.push_back(Path::GetFileNameWithoutExtension(se));
			}
		}
		
		return test;
	}, [this](int32_t index, const String& item) {
		if (index >= 0)
		{
			m_se = Singleton<AssetManager>::GetInstance()->Load<SE>(item);
		}
	});
	menu->SetWidget(choice);

	auto play = SystemWidgetCustom::Create(u8"�Đ�/��~", [this]()->bool {
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

	auto pause = SystemWidgetCustom::Create(u8"�ꎞ��~/�ĊJ", [this]()->bool {
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

	auto vol = SystemWidgetSpin::Create(u8"�{�����[��", [this](int32_t val) {
		m_volume = val;
	});
	vol->SetRange(0, 100, 1);
	vol->SetValue(100);
	menu->SetWidget(vol);

	menu->SetFocus(true);

	return 0;
}

int SystemViewSETest::Do()
{
	if (m_se)
	{
		m_se->SetVolume(m_volume / 100.f);
	}
	return 0;
}

int SystemViewSETest::Render()
{
	return 0;
}

