#include "system/Logger.h"
#include "system/Exception.hpp"
#include "SystemView.hpp"
#include "Application.hpp"

SystemView::SystemView()
{

}

SystemView::~SystemView()
{

}

void SystemView::SetPos(const Point& pos)
{

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
	for (auto& widget : m_vWidget)
	{
		//if (widget->GetFocus())
		{
			widget->Prepare();
			widget->Do();
		}
	}

	Do();
}

void SystemView::RenderView()
{
	Render();

	for (auto& widget : m_vWidget)
	{
		widget->Render();
	}
}

class TopMenu : public SystemView
{
public:
	TopMenu() = default;
	virtual ~TopMenu() = default;

	int Enter() override
	{
		m_returnToGame = SystemWidgetSpin::Create("RETURN TO GAME", [](int index) {
		});
		m_vWidget.push_back(m_returnToGame);

		//m_returnToGame = SystemWidgetEnterView::Create("RETURN TO GAME");
		//m_vWidget.push_back(m_returnToGame);

		//m_inputTest = SystemWidgetEnterView::Create("INPUT TEST");
		//m_vWidget.push_back(m_inputTest);

		//m_assetTest = SystemWidgetEnterView::Create("ASSET TEST");
		//m_vWidget.push_back(m_assetTest);

		m_returnToGame->SetFocus(true);

		return 0;
	}

	//void Do() override;

	std::shared_ptr<TopMenu> Create();

protected:
	std::shared_ptr<SystemWidget> m_returnToGame;
	std::shared_ptr<SystemWidget> m_inputTest;
	std::shared_ptr<SystemWidget> m_assetTest;
};

//void TopMenu::Do()
//{
//	ウィジェットの操作
//}

std::shared_ptr<AssetMenu> AssetMenu::Create()
{
	auto p = std::make_shared<AssetMenu>();

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

	auto rate = SystemWidgetSpin::Create(u8"拡大率(%)", [this](int32_t val) {
		m_rate = val;
	});
	rate->SetFocus(true);
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

	//auto m_prev = SystemWidgetReturnView::Create("RETURN");
	//m_vWidget.push_back(m_prev);

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
	m_spriteRenderer->SetScale(m_rate / 100.f, m_rate / 100.f);
	m_spriteRenderer->SetAtlasNum(m_ptr);
	m_spriteRenderer->SetPos(m_spritePos);

	return 0;
}

int AssetMenu::Render()
{
	GetApplication()->GetRenderer()->AddRenderQueue(m_spriteRenderer.get());

	return 0;
}


