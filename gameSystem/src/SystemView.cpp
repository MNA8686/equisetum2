#include "system/Logger.h"
#include "system/Exception.hpp"
#include "SystemView.hpp"
#include "Application.hpp"

#if 0
std::shared_ptr<SystemView> SystemView::CreateSystemView()
{
	EQ_DURING
	{
		auto inst = std::shared_ptr<SystemView>(new SystemView);
		if (!inst)
		{
			EQ_THROW(u8"インスタンスの作成に失敗しました。");
		}


		return inst;
	}
	EQ_HANDLER
	{
		Logger::OutputError(EQ_GET_HANDLER().what());
	}
	EQ_END_HANDLER

	return nullptr;
}
#endif

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
	size_t index = 0;
	size_t nextFocus = 0;
	bool enable = true;

	for (auto& widget : m_vWidget)
	{
		if (widget->GetFocus())
		{
			widget->Prepare();
			widget->Do();

			SystemWidget::Stat stat = widget->GetStat();
			switch (stat)
			{
			case SystemWidget::Stat::Prev:
				nextFocus = (index == 0 ? m_vWidget.size() : index) - 1;
				break;
			case SystemWidget::Stat::Next:
				nextFocus = (index + 1) % m_vWidget.size();
				break;
			default:
				nextFocus = index;
				break;
			}

			if (index != nextFocus)
			{
				widget->SetFocus(false);
				m_vWidget[nextFocus]->SetFocus(true);
			}

			if (widget->GetStat() == SystemWidget::Stat::Exclusive)
			{
				enable = false;
			}
			break;
		}

		index++;
	}

	for (auto& widget : m_vWidget)
	{
		widget->SetEnable(enable);
	}
	
	Do();
}

void SystemView::RenderView()
{
	Render();

	for (auto& widget : m_vWidget)
	{
		if (widget->GetFocus())
		{
			// ハイライト処理とか
		}

		widget->Render();
		widget->RenderLabel();
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

	auto m_return = SystemWidgetSpin::Create(u8"拡大率", [this](int32_t val) {
		m_rate = val;
	});
	m_return->SetPos({ 0.05f, 0.2f });
	m_return->SetFocus(true);
	m_return->SetRange(10, 1000, 10);
	m_return->SetValue(100);
	m_vWidget.push_back(m_return);

	auto m_return2 = SystemWidgetSpin::Create(u8"アニメーションパターン", [this](int32_t val) {
		m_ptr = val;
	});
	m_return2->SetPos({ 0.05f, 0.25f });
	m_return2->SetRange(0, sprite->GetAnimAtlas().size(), 1);
	m_return2->SetCyclic(true);
	m_vWidget.push_back(m_return2);

	auto m_return3 = SystemWidgetCustom::Create(u8"移動", [this]()->bool {
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
	m_return3->SetPos({ 0.05f, 0.30f });
	m_vWidget.push_back(m_return3);

	//auto m_prev = SystemWidgetReturnView::Create("RETURN");
	//m_vWidget.push_back(m_prev);

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


