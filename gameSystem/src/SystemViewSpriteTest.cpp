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

std::shared_ptr<SystemViewSpriteTest> SystemViewSpriteTest::Create(const String & name)
{
	auto p = std::make_shared<SystemViewSpriteTest>();

	p->m_name = name;
	p->Enter();

	return p;
}

void SystemViewSpriteTest::LoadSprite()
{
	if (m_item.empty())
	{
		return;
	}

	m_spriteRenderer = nullptr;
	m_ptr = 0;
	m_tagIndex = -1;		// ���[�h���͕K���^�O���ݒ��ԂƂ���
	m_spinTag->SetRange(-1, -1, 1);
	m_spinTag->SetValue(-1);

	// �X�v���C�g�쐬
	std::shared_ptr<Sprite> sprite = Singleton<AssetManager>::GetInstance()->Load<Sprite>(m_item);
	if (sprite)
	{
		// �X�v���C�g�����_���쐬
		m_spriteRenderer = GetApplication()->GetRenderer()->CreateRenderObject<SpriteRenderer>();
		if (m_spriteRenderer)
		{
			m_spriteRenderer->SetSprite(sprite);
			m_spriteRenderer->SetBlendMode(BlendMode::Blend);
			m_spritePos = Window::Size() / 2;
			m_spriteRenderer->SetPos(m_spritePos);

			// �A�j���[�V�����p�^�[�����ݒ�
			size_t animSize = sprite->GetAnimAtlas().size();
			m_spinAnim->SetRange(0, animSize > 0 ? animSize - 1 : 0, 1);

			// �^�O�I���X�s���ݒ�
			size_t tagsSize = sprite->GetTags().size();
			m_spinTag->SetRange(-1, tagsSize > 0 ? tagsSize - 1 : -1, 1);
			m_spinTag->SetValue(-1);
		}
	}
}

int SystemViewSpriteTest::Enter()
{
	auto menu = SystemWidgetMenu::Create(u8"���j���[");
	menu->SetPos({ 0.05f, 0.2f });
	m_vWidget.push_back(menu);

	auto pop = SystemWidgetPopView::Create(u8"�߂�");
	menu->SetWidget(pop);

	auto choice = SystemWidgetChoice::Create(u8"�t�@�C���I��", [this]()->std::vector<String> {
		std::vector<String> test;
		
		// �X�v���C�g���X�g���擾����
		auto spriteList = Singleton<AssetManager>::GetInstance()->GetIdList("sprite");
		for (auto sprite : spriteList)
		{
			if (Path::GetExtension(sprite) == u8".json")
			{
				test.push_back(Path::ChangeExtension(sprite, ""));
			}
		}
		
		return test;
	}, [this](int32_t index, const String& item) {
		if (index >= 0)
		{
			m_item = item;
			LoadSprite();
		}
	});
	menu->SetWidget(choice);

	auto play = SystemWidgetCustom::Create(u8"�����[�h", [this]()->bool {
		LoadSprite();
		return false;
	});
	menu->SetWidget(play);

	auto rate = SystemWidgetSpin::Create(u8"�g�嗦", [this](int32_t val) {
		m_rate = val;
	});
	rate->SetRange(10, 1000, 10);
	rate->SetValue(100);
	rate->SetFormatCallBack([](int32_t val)->String {
		return String::Sprintf("%d%%", val);
	});
	menu->SetWidget(rate);

	auto tag = SystemWidgetSpin::Create(u8"�^�O", [this](int32_t val) {
		// �^�O�ؑ֎��̃R�[���o�b�N
		m_tagIndex = val;
		m_ptr = 0;

		if (m_spriteRenderer &&
			m_spriteRenderer->GetSprite())
		{
			auto sprite = m_spriteRenderer->GetSprite();
			// 0�����̏ꍇ�͑S�̂̃A�j���[�V�����p�^�[�������A0�ȏ�̏ꍇ�͑Ή�����^�O�̃T�C�Y���Z�b�g
			int32_t max = val < 0 ? sprite->GetAnimAtlas().size() : sprite->GetAtlasSizeByTagIndex(val);
			
			m_spinAnim->SetRange(0, max - 1, 1);
		}
		m_spinAnim->SetValue(m_ptr);
	});
	tag->SetCyclic(true);
	tag->SetEnable(true);
	tag->SetRange(-1, -1, 1);
	tag->SetValue(-1);
	tag->SetFormatCallBack([this](int32_t val)->String {
		if (val < 0)
		{
			return "* ALL *";
		}

		if (m_spriteRenderer &&
			m_spriteRenderer->GetSprite())
		{
			// �^�O���������Ă��Ă����߂�l�ŕԂ�
			return m_spriteRenderer->GetSprite()->GetTags()[val].tag;
		}

		return "???";
	});
	m_spinTag = tag;
	menu->SetWidget(tag);

	auto ptr = SystemWidgetSpin::Create(u8"�A�j���[�V�����p�^�[��", [this](int32_t val) {
		m_ptr = val;
	});
	m_spinAnim = ptr;
	ptr->SetCyclic(true);
	ptr->SetEnable(false);
	ptr->SetFormatCallBack([this](int32_t val)->String {
		return String::Sprintf("%d / %d", val, m_spinAnim->GetMax() + 1);
	});
	menu->SetWidget(ptr);

	auto r = SystemWidgetSpin::Create(u8"�F���� R", [this](int32_t val) {
		m_color.rgba8888.r = static_cast<uint8_t>(val + 128);
	});
	r->SetRange(-128, 127, 1);
	r->SetValue(0);
	menu->SetWidget(r);

	auto g = SystemWidgetSpin::Create(u8"�F���� G", [this](int32_t val) {
		m_color.rgba8888.g = static_cast<uint8_t>(val + 128);
	});
	g->SetRange(-128, 127, 1);
	g->SetValue(0);
	menu->SetWidget(g);

	auto b = SystemWidgetSpin::Create(u8"�F���� B", [this](int32_t val) {
		m_color.rgba8888.b = static_cast<uint8_t>(val + 128);
	});
	b->SetRange(-128, 127, 1);
	b->SetValue(0);
	menu->SetWidget(b);

	auto a = SystemWidgetSpin::Create(u8"�F���� A", [this](int32_t val) {
		m_color.rgba8888.a = static_cast<uint8_t>(val + 128);
	});
	a->SetRange(-128, 127, 1);
	a->SetValue(0);
	menu->SetWidget(a);

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

int SystemViewSpriteTest::Do()
{
	return 0;
}

int SystemViewSpriteTest::Render()
{
	if (m_spriteRenderer)
	{
		m_spriteRenderer->SetScale(m_rate / 100.f, m_rate / 100.f);
		m_spriteRenderer->SetAtlasNum(m_spriteRenderer->GetSprite()->ToAtlasNumWithTagIndex(m_tagIndex, m_ptr));
		m_spriteRenderer->SetPos(PosNormalToPixel() + m_spritePos);
		m_spriteRenderer->SetColor(m_color);

		GetApplication()->GetRenderer()->AddRenderQueue(m_spriteRenderer.get());
	}

	return 0;
}

