#include "system/Logger.h"
#include "system/Exception.hpp"
#include "SystemViewSpriteTest.hpp"
#include "SystemWidgetMenu.hpp"
#include "SystemWidgetSpin.hpp"
#include "SystemWidgetPopView.hpp"
#include "SystemWidgetPushView.hpp"
#include "SystemWidgetCustom.hpp"
#include "SystemWidgetChoice.hpp"
#include "Application.hpp"

std::shared_ptr<SpriteTest> SpriteTest::Create(const String & name)
{
	auto p = std::make_shared<SpriteTest>();

	p->m_name = name;
	p->Enter();

	return p;
}

int SpriteTest::Enter()
{
	auto menu = SystemWidgetMenu::Create(u8"���j���[");
	menu->SetPos({ 0.05f, 0.2f });
	m_vWidget.push_back(menu);

	auto pop = SystemWidgetPopView::Create(u8"�߂�");
	menu->SetWidget(pop);

	auto choice = SystemWidgetChoice::Create(u8"�t�@�C���I��", [this]()->std::vector<String> {
		std::vector<String> test;
		
		// �X�v���C�g���X�g���擾����
		auto spriteList = Directory::GetFiles(Path::GetFullPath("sprite"));
		if (spriteList)
		{
			for (auto sprite : *spriteList)
			{
				test.push_back(Path::GetFileNameWithoutExtension(sprite));
			}
		}
		
		return test;
	}, [this](int32_t index, const String& item) {
		if (index >= 0)
		{
			// �X�v���C�g�����_���쐬
			m_spriteRenderer = std::dynamic_pointer_cast<SpriteRenderer>(GetApplication()->GetRenderer()->CreateRenderObject(RenderType::SPRITE));
			if (m_spriteRenderer)
			{
				// �X�v���C�g�쐬
				std::shared_ptr<Sprite> sprite = Singleton<AssetManager>::GetInstance()->Load<Sprite>(item);
				if (sprite)
				{
					m_spriteRenderer->SetSprite(sprite);
					m_spriteRenderer->SetBlendMode(BlendMode::Blend);
					m_spritePos = Window::Size() / 2;
					m_spriteRenderer->SetPos(m_spritePos);

					// �A�j���[�V�����p�^�[�����ݒ�
					size_t animSize = sprite->GetAnimAtlas().size();
					m_spinAnim->SetRange(0, animSize > 0 ? animSize - 1 : 0, 1);
				}
			}
		}
	});
	menu->SetWidget(choice);

	auto rate = SystemWidgetSpin::Create(u8"�g�嗦(%)", [this](int32_t val) {
		m_rate = val;
	});
	rate->SetRange(10, 1000, 10);
	rate->SetValue(100);
	menu->SetWidget(rate);

	auto ptr = SystemWidgetSpin::Create(u8"�A�j���[�V�����p�^�[��", [this](int32_t val) {
		m_ptr = val;
	});
	ptr->SetCyclic(true);
	ptr->SetEnable(false);
	m_spinAnim = ptr;
	menu->SetWidget(ptr);

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

	menu->SetFocus(true);

	return 0;
}

int SpriteTest::Do()
{
	return 0;
}

int SpriteTest::Render()
{
	if (m_spriteRenderer)
	{
		m_spriteRenderer->SetScale(m_rate / 100.f, m_rate / 100.f);
		m_spriteRenderer->SetAtlasNum(m_ptr);
		m_spriteRenderer->SetPos(PosNormalToPixel() + m_spritePos);

		GetApplication()->GetRenderer()->AddRenderQueue(m_spriteRenderer.get());
	}

	return 0;
}

