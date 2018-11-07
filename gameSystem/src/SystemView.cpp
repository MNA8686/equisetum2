#include "system/Logger.h"
#include "system/Exception.hpp"
#include "SystemWidgetMenu.hpp"
#include "SystemWidgetCustom.hpp"
#include "SystemWidgetLabel.hpp"
#include "SystemWidgetSpin.hpp"
#include "SystemWidgetPushView.hpp"
#include "SystemWidgetPopView.hpp"
#include "SystemWidgetChoice.hpp"
#include "SystemView.hpp"
#include "Application.hpp"

void SystemView::SetPos(const PointF& pos)
{
	m_pos = pos;
}

PointF SystemView::GetPos() const
{
	return m_pos;
}

Point SystemView::PosNormalToPixel() const
{
	Size size = Window::Size();
	return { static_cast<int32_t>(size.x * m_pos.x), static_cast<int32_t>(size.y * m_pos.y) };
}

const String& SystemView::GetName() const
{
	return m_name;
}

int SystemView::Enter()
{
	return 0;
}

int SystemView::Leave()
{
	return 0;
}

int SystemView::Do()
{
	return 0;
}

int SystemView::Render()
{
	return 0;
}

void SystemView::DoView()
{
	m_stat = Stat::Idle;

	for (auto& widget : m_vWidget)
	{
		if (widget->GetFocus())
		{
			widget->Prepare();
			widget->Do(this);
			break;
		}
	}

	Do();
}

void SystemView::RenderView()
{
	Render();

	for (auto& widget : m_vWidget)
	{
		widget->Render(this);
	}
}

void SystemView::Push(std::shared_ptr<SystemView> pView)
{
	m_stat = Stat::Push;
	m_nextView = pView;
}

std::shared_ptr<SystemView> SystemView::GetNextView()
{
	return m_nextView;
}

void SystemView::Pop()
{
	m_stat = Stat::Pop;
}

void SystemView::SetFocus(bool focus)
{
	m_focus = focus;
}

bool SystemView::GetFocus() const
{
	return m_focus;
}

SystemView::Stat SystemView::GetStat() const
{
	return m_stat;
}




class TestMenu : public SystemView
{
public:
	TestMenu() = default;
	virtual ~TestMenu() = default;

	int Enter() override
	{
		auto menu = SystemWidgetMenu::Create(u8"メニュー");
		menu->SetPos({ 0.05f, 0.2f });
		m_vWidget.push_back(menu);

//		auto test = SystemWidgetSpin::Create("test", [](int index) {
//		});
//		menu->SetWidget(test);

		auto returnToGame = SystemWidgetPopView::Create("testdayo");
		menu->SetWidget(returnToGame);

		menu->SetFocus(true);

		return 0;
	}

	//void Do() override;

	static std::shared_ptr<TestMenu> Create(const String& name);

protected:
};

std::shared_ptr<TestMenu> TestMenu::Create(const String & name)
{
	auto p = std::make_shared<TestMenu>();

	p->m_name = name;
	p->Enter();

	return p;
}



class TopMenu : public SystemView
{
public:
	TopMenu() = default;
	virtual ~TopMenu() = default;

	int Enter() override
	{
		auto menu = SystemWidgetMenu::Create(u8"メニュー");
		menu->SetPos({ 0.05f, 0.2f });
		m_vWidget.push_back(menu);

		auto test = SystemWidgetSpin::Create("test", [](int index) {
		});
		menu->SetWidget(test);

		auto returnToGame = SystemWidgetPopView::Create("RETURN TO GAME");
		menu->SetWidget(returnToGame);

		auto next = SystemWidgetPushView::Create("NEXT", [this]()->std::shared_ptr<SystemView> {
			return TestMenu::Create(u8"その次の画面");
		});
		menu->SetWidget(next);

		menu->SetFocus(true);

		return 0;
	}

	//void Do() override;

	static std::shared_ptr<TopMenu> Create(const String& name);

protected:
	std::shared_ptr<SystemWidget> m_returnToGame;
	std::shared_ptr<SystemWidget> m_inputTest;
	std::shared_ptr<SystemWidget> m_assetTest;
};

std::shared_ptr<TopMenu> TopMenu::Create(const String & name)
{
	auto p = std::make_shared<TopMenu>();

	p->m_name = name;
	p->Enter();

	return p;
}
//void TopMenu::Do()
//{
//	ウィジェットの操作
//}

std::shared_ptr<AssetMenu> AssetMenu::Create(const String & name)
{
	auto p = std::make_shared<AssetMenu>();

	p->m_name = name;
	p->Enter();

	return p;
}

int AssetMenu::Enter()
{
	m_spriteRenderer = std::dynamic_pointer_cast<SpriteRenderer>(GetApplication()->GetRenderer()->CreateRenderObject(RenderType::SPRITE));

	std::shared_ptr<Sprite> sprite = Singleton<AssetManager>::GetInstance()->Load<Sprite>("bullet_tama08");
	m_spriteRenderer->SetSprite(sprite);
	m_spriteRenderer->SetBlendMode(BlendMode::Blend);
	m_spritePos = Window::Size() / 2;
	m_spriteRenderer->SetPos(m_spritePos);

	auto menu = SystemWidgetMenu::Create(u8"メニュー");
	menu->SetPos({ 0.05f, 0.2f });
	m_vWidget.push_back(menu);

	auto returnToGame = SystemWidgetPopView::Create(u8"ゲームへ戻る");
	menu->SetWidget(returnToGame);

	auto rate = SystemWidgetSpin::Create(u8"拡大率(%)", [this](int32_t val) {
		m_rate = val;
	});
	rate->SetRange(10, 1000, 10);
	rate->SetValue(100);
	menu->SetWidget(rate);

	auto ptr = SystemWidgetSpin::Create(u8"アニメーションパターン", [this](int32_t val) {
		m_ptr = val;
	});
	size_t animSize = sprite->GetAnimAtlas().size();
	ptr->SetRange(0, animSize > 0 ? animSize - 1 : 0, 1);
	ptr->SetCyclic(true);
	menu->SetWidget(ptr);

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

		return !KB::KeyZ.IsDown();
	});
	menu->SetWidget(move);

	auto next = SystemWidgetPushView::Create(u8"次の画面へ", [this]()->std::shared_ptr<SystemView> {
		return TopMenu::Create(u8"次の画面");
	});
	menu->SetWidget(next);

	auto choice = SystemWidgetChoice::Create(u8"ファイル選択", [this]()->std::vector<String> {
		return{ u8"test", u8"test2" };
	});
	menu->SetWidget(choice);

	menu->SetFocus(true);

	{
		auto ptr = SystemWidgetSpin::Create(u8"テストだよ", [this](int32_t val) {
		});
		ptr->SetPos({ 0.05f, 0.75f });
		m_vWidget.push_back(ptr);
	}
	{
		auto ptr = SystemWidgetLabel::Create(u8"テストだよん");
		ptr->SetPos({ 1.f, 0.5f });
		ptr->SetPivot({ 1.f, 0.5f });
		m_vWidget.push_back(ptr);
	}
	return 0;
}

int AssetMenu::Do()
{
	return 0;
}

int AssetMenu::Render()
{
	m_spriteRenderer->SetScale(m_rate / 100.f, m_rate / 100.f);
	m_spriteRenderer->SetAtlasNum(m_ptr);
	m_spriteRenderer->SetPos(PosNormalToPixel() + m_spritePos);

	GetApplication()->GetRenderer()->AddRenderQueue(m_spriteRenderer.get());

	return 0;
}

