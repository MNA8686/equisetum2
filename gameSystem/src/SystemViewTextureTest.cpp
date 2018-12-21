#include "system/Logger.h"
#include "system/Exception.hpp"
#include "SystemViewTextureTest.hpp"
#include "SystemWidgetMenu.hpp"
#include "SystemWidgetSpin.hpp"
#include "SystemWidgetPopView.hpp"
#include "SystemWidgetPushView.hpp"
#include "SystemWidgetCustom.hpp"
#include "SystemWidgetChoice.hpp"
#include "Application.hpp"

std::shared_ptr<SystemViewTextureTest> SystemViewTextureTest::Create(const String & name)
{
	auto p = std::make_shared<SystemViewTextureTest>();

	p->m_name = name;
	p->Enter();

	return p;
}

int SystemViewTextureTest::Enter()
{
	auto menu = SystemWidgetMenu::Create(u8"メニュー");
	menu->SetPos({ 0.05f, 0.2f });
	m_vWidget.push_back(menu);

	auto pop = SystemWidgetPopView::Create(u8"戻る");
	menu->SetWidget(pop);

	auto choice = SystemWidgetChoice::Create(u8"ファイル選択", [this]()->std::vector<String> {
		std::vector<String> test;
		
		// スプライトリストを取得する
		auto spriteList = Singleton<AssetManager>::GetInstance()->GetIdList("image");
		for (auto sprite : spriteList)
		{
			if (Path::GetExtension(sprite) == u8".png")
			{
				test.push_back(Path::GetFileNameWithoutExtension(sprite));
			}
		}
		
		return test;
	}, [this](int32_t index, const String& item) {
		if (index >= 0)
		{
			// スプライトレンダラ作成
			m_spriteRenderer = GetApplication()->GetRenderer()->CreateRenderObject<SpriteRenderer>();
			if (m_spriteRenderer)
			{
				// スプライト作成
				std::shared_ptr<Texture> texture = Singleton<AssetManager>::GetInstance()->Load<Texture>(item);
				if (texture)
				{
					auto sprite = Sprite::CreateFromTexture(texture);

					m_spriteRenderer->SetSprite(sprite);
					m_spriteRenderer->SetBlendMode(BlendMode::Blend);
					m_spritePos = Window::Size() / 2;
					m_spriteRenderer->SetPos(m_spritePos);
				}
			}
		}
	});
	menu->SetWidget(choice);

	auto rate = SystemWidgetSpin::Create(u8"拡大率(%)", [this](int32_t val) {
		m_rate = val;
	});
	rate->SetRange(10, 1000, 10);
	rate->SetValue(100);
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

int SystemViewTextureTest::Do()
{
	return 0;
}

int SystemViewTextureTest::Render()
{
	if (m_spriteRenderer)
	{
		m_spriteRenderer->SetScale(m_rate / 100.f, m_rate / 100.f);
		m_spriteRenderer->SetPos(PosNormalToPixel() + m_spritePos);
		m_spriteRenderer->SetColor(m_color);

		GetApplication()->GetRenderer()->AddRenderQueue(m_spriteRenderer.get());
	}

	return 0;
}

