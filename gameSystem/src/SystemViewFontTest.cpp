#include "system/Logger.h"
#include "system/Exception.hpp"
#include "SystemViewFontTest.hpp"
#include "SystemWidgetMenu.hpp"
#include "SystemWidgetSpin.hpp"
#include "SystemWidgetPopView.hpp"
#include "SystemWidgetPushView.hpp"
#include "SystemWidgetCustom.hpp"
#include "SystemWidgetChoice.hpp"
#include "Application.hpp"

static const char* testString = u8"abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ\n1234567890.:,;'\"(!?)+-*/=\nEquisetum2でゲームの世界が広がります。";

std::shared_ptr<SystemViewFontTest> SystemViewFontTest::Create(const String & name)
{
	auto p = std::make_shared<SystemViewFontTest>();

	p->m_name = name;
	p->Enter();

	return p;
}

void SystemViewFontTest::LoadFont()
{
	if (m_item.empty())
	{
		return;
	}

	// フォント名とフォントサイズを設定
	String arg = String::Sprintf(u8"%s?%d", m_item.c_str(), m_fontSize);
	// フォントマネージャーを作成する
	auto fontManager = Singleton<AssetManager>::GetInstance()->Load<FontManager>(arg);

	m_label->SetPreset(testString, fontManager);
	m_label->SetText(testString);
}

int SystemViewFontTest::Enter()
{
	auto menu = SystemWidgetMenu::Create(u8"メニュー");
	menu->SetPos({ 0.05f, 0.2f });
	m_vWidget.push_back(menu);

	auto pop = SystemWidgetPopView::Create(u8"戻る");
	menu->SetWidget(pop);

	auto choice = SystemWidgetChoice::Create(u8"ファイル選択", [this]()->std::vector<String> {
		std::vector<String> test;
		
		// フォントリストを取得する
		//auto fontList = Directory::GetFiles(Path::GetFullPath("font"));
		auto fontList = Singleton<AssetManager>::GetInstance()->GetIdList("font");
		for (auto font : fontList)
		{
			if (Path::GetExtension(font) == u8".ttf")
			{
				test.push_back(Path::GetFileNameWithoutExtension(font));
			}
		}
		
		return test;
	}, [this](int32_t index, const String& item) {
		if (index >= 0)
		{
			m_item = item;
			LoadFont();
		}
	});
	menu->SetWidget(choice);

	auto play = SystemWidgetCustom::Create(u8"リロード", [this]()->bool {
		LoadFont();
		return false;
	});
	menu->SetWidget(play);

	auto rate = SystemWidgetSpin::Create(u8"フォントサイズ", [this](int32_t val) {
		m_fontSize = val;
		LoadFont();
	});
	rate->SetRange(8, 72, 2);
	rate->SetValue(16);
	menu->SetWidget(rate);

	auto r = SystemWidgetSpin::Create(u8"色成分 R", [this](int32_t val) {
		m_color.rgba8888.r = static_cast<uint8_t>(val + 128);
	});
	r->SetRange(-128, 127, 1);
	r->SetValue(0);
	menu->SetWidget(r);

	auto g = SystemWidgetSpin::Create(u8"色成分 G", [this](int32_t val) {
		m_color.rgba8888.g = static_cast<uint8_t>(val + 128);
	});
	g->SetRange(-128, 127, 1);
	g->SetValue(0);
	menu->SetWidget(g);

	auto b = SystemWidgetSpin::Create(u8"色成分 B", [this](int32_t val) {
		m_color.rgba8888.b = static_cast<uint8_t>(val + 128);
	});
	b->SetRange(-128, 127, 1);
	b->SetValue(0);
	menu->SetWidget(b);

	auto a = SystemWidgetSpin::Create(u8"色成分 A", [this](int32_t val) {
		m_color.rgba8888.a = static_cast<uint8_t>(val + 128);
	});
	a->SetRange(-128, 127, 1);
	a->SetValue(0);
	menu->SetWidget(a);

	auto move = SystemWidgetCustom::Create(u8"移動", [this]()->bool {
		float amount = KB::KeyTab.IsPress() ? 0.04f : 0.01f;

		if (KB::KeyLeft.IsPress())
		{
			m_textPos.x -= amount;
		}
		else if (KB::KeyRight.IsPress())
		{
			m_textPos.x += amount;
		}

		if (KB::KeyUp.IsPress())
		{
			m_textPos.y -= amount;
		}
		else if (KB::KeyDown.IsPress())
		{
			m_textPos.y += amount;
		}

		return !KB::KeyEscape.IsDown();
	});
	menu->SetWidget(move);

	menu->SetFocus(true);

	m_label = SystemWidgetLabel::Create(testString);
	m_label->SetPivot({0, 0.5f});
	m_label->SetPostEffect([this](TextRenderer* renderer) {
		renderer->SetColor(m_color);
	});

	return 0;
}

int SystemViewFontTest::Do()
{
	m_label->SetPos(m_textPos);
	return 0;
}

int SystemViewFontTest::Render()
{
	if (!m_item.empty())
	{
		m_label->Render(this);
	}

	return 0;
}

