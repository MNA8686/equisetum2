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
#include "SystemViewTopMenu.hpp"
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
		auto menu = SystemWidgetMenu::Create(u8"ƒƒjƒ…[");
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



