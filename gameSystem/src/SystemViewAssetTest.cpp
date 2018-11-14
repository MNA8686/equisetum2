#include "system/Logger.h"
#include "system/Exception.hpp"
#include "SystemViewAssetTest.hpp"
#include "SystemViewSpriteTest.hpp"
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

	auto spriteTest = SystemWidgetPushView::Create(u8"�X�v���C�g�e�X�g", [this]()->std::shared_ptr<SystemView> {
		return SpriteTest::Create(u8"�X�v���C�g�e�X�g");
	});
	menu->SetWidget(spriteTest);

	auto bgmTest = SystemWidgetPushView::Create(u8"BGM�e�X�g", [this]()->std::shared_ptr<SystemView> {
		return{};// AssetTest::Create(u8"�A�Z�b�g�e�X�g");
	});
	menu->SetWidget(bgmTest);

	auto seTest = SystemWidgetPushView::Create(u8"SE�e�X�g", [this]()->std::shared_ptr<SystemView> {
		return{};// AssetTest::Create(u8"�A�Z�b�g�e�X�g");
	});
	menu->SetWidget(seTest);

	menu->SetFocus(true);

#if 0
	m_spriteRenderer = std::dynamic_pointer_cast<SpriteRenderer>(GetApplication()->GetRenderer()->CreateRenderObject(RenderType::SPRITE));

	std::shared_ptr<Sprite> sprite = Singleton<AssetManager>::GetInstance()->Load<Sprite>("bullet_tama08");
	m_spriteRenderer->SetSprite(sprite);
	m_spriteRenderer->SetBlendMode(BlendMode::Blend);
	m_spritePos = Window::Size() / 2;
	m_spriteRenderer->SetPos(m_spritePos);

	auto menu = SystemWidgetMenu::Create(u8"���j���[");
	menu->SetPos({ 0.05f, 0.2f });
	m_vWidget.push_back(menu);

	auto returnToGame = SystemWidgetPopView::Create(u8"�Q�[���֖߂�");
	menu->SetWidget(returnToGame);

	auto rate = SystemWidgetSpin::Create(u8"�g�嗦(%)", [this](int32_t val) {
		m_rate = val;
	});
	rate->SetRange(10, 1000, 10);
	rate->SetValue(100);
	menu->SetWidget(rate);

	auto ptr = SystemWidgetSpin::Create(u8"�A�j���[�V�����p�^�[��", [this](int32_t val) {
		m_ptr = val;
	});
	size_t animSize = sprite->GetAnimAtlas().size();
	ptr->SetRange(0, animSize > 0 ? animSize - 1 : 0, 1);
	ptr->SetCyclic(true);
	menu->SetWidget(ptr);

	auto choice = SystemWidgetChoice::Create(u8"�t�@�C���I��", [this]()->std::vector<String> {
		std::vector<String> test;
		
		auto spriteList = Directory::GetFiles(Path::GetFullPath("sprite"));
		if (spriteList)
		{
			for (auto sprite : *spriteList)
			{
				test.push_back(Path::GetFileNameWithoutExtension(sprite));
			}
		}
		
		return test;
	});
	menu->SetWidget(choice);

	auto move = SystemWidgetCustom::Create(u8"�ړ�", [this]()->bool {
		int32_t amount = KB::KeyTab.IsPress() ? 4 : 1;

		if (KB::KeyLeft.IsPress())
		{
			m_spritePos.x -= amount;
		}
		else if (KB::KeyRight.IsPress())
		{
			m_spritePos.x += amount;
		}

		if (KB::KeyUp.IsPress())
		{
			m_spritePos.y -= amount;
		}
		else if (KB::KeyDown.IsPress())
		{
			m_spritePos.y += amount;
		}

		return !KB::KeyEscape.IsDown();
	});
	menu->SetWidget(move);

	//auto next = SystemWidgetPushView::Create(u8"���̉�ʂ�", [this]()->std::shared_ptr<SystemView> {
	//	return TopMenu::Create(u8"���̉��");
	//});
	//menu->SetWidget(next);

	menu->SetFocus(true);
#endif
	return 0;
}

int AssetTest::Do()
{
	return 0;
}

int AssetTest::Render()
{
	//m_spriteRenderer->SetScale(m_rate / 100.f, m_rate / 100.f);
	//m_spriteRenderer->SetAtlasNum(m_ptr);
	//m_spriteRenderer->SetPos(PosNormalToPixel() + m_spritePos);

	//GetApplication()->GetRenderer()->AddRenderQueue(m_spriteRenderer.get());

	return 0;
}

