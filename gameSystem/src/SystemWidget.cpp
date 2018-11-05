#include "SystemWidget.hpp"
#include "Application.hpp"

SystemWidget::SystemWidget()
{
}

SystemWidget::~SystemWidget()
{
}

void SystemWidget::Prev()
{
	m_stat = Stat::Prev;
}

void SystemWidget::Next()
{
	m_stat = Stat::Next;
}

void SystemWidget::SetFocus(bool focus)
{
	m_focus = focus;
}

bool SystemWidget::GetFocus() const
{
	return m_focus;
}

SystemWidget::Stat SystemWidget::GetStat() const
{
	return m_stat;
}

void SystemWidget::Prepare()
{
	m_stat = Stat::Idle;
}

void SystemWidget::SetPos(const PointF& pos)
{
	m_pos = pos;
}

PointF SystemWidget::GetPos() const
{
	return m_pos;
}

void SystemWidget::SetEnable(bool enable)
{
	m_enable = enable;
}









std::shared_ptr<SystemWidgetPopView> SystemWidgetPopView::Create(const String& label)
{
	EQ_DURING
	{
		auto inst = std::shared_ptr<SystemWidgetPopView>(new SystemWidgetPopView);
		if (!inst)
		{
			EQ_THROW(u8"インスタンスの作成に失敗しました。");
		}

		inst->m_text = label;

		inst->m_label = SystemWidgetLabel::Create(label);
		if (!inst->m_label)
		{
			EQ_THROW(u8"ラベルの作成に失敗しました。");
		}
		inst->m_label->SetPreset(u8" " + label);

		if (!inst->m_label->SetText(label))
		{
			EQ_THROW(u8"ラベルの作成に失敗しました。");
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

int SystemWidgetPopView::Do(SystemView* pView)
{
	// Zキー押下？
	if (KB::KeyEnter.IsDown())
	{
		pView->Pop();
	}
	// 上キー押下？
	else if (KB::KeyUp.IsDown())
	{
		Prev();
	}
	// 下キー押下？
	else if (KB::KeyDown.IsDown())
	{
		Next();
	}

	return 0;
}

int SystemWidgetPopView::Render(const SystemView* pView)
{
	m_label->Render(pView);
	// label
	return 0;
}

Rect SystemWidgetPopView::GetBox() const
{
	return m_label->GetBox();
}

void SystemWidgetPopView::SetPos(const PointF & pos)
{
	SystemWidget::SetPos(pos);
	m_label->SetPos(pos);
}



std::shared_ptr<SystemWidgetPushView> SystemWidgetPushView::Create(const String& label, const std::function<std::shared_ptr<SystemView>()>& cb)
{
	EQ_DURING
	{
		auto inst = std::shared_ptr<SystemWidgetPushView>(new SystemWidgetPushView);
		if (!inst)
		{
			EQ_THROW(u8"インスタンスの作成に失敗しました。");
		}

		inst->m_cb = cb;
		inst->m_label = SystemWidgetLabel::Create(label);
		if (!inst->m_label)
		{
			EQ_THROW(u8"ラベルの作成に失敗しました。");
		}
		inst->m_label->SetPreset(u8" " + label);

		if (!inst->m_label->SetText(label))
		{
			EQ_THROW(u8"ラベルの作成に失敗しました。");
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

int SystemWidgetPushView::Do(SystemView* pView)
{
	// Enterキー押下？
	if (KB::KeyEnter.IsDown())
	{
		if (m_cb)
		{
			pView->Push(m_cb());
		}
	}
	// 上キー押下？
	else if (KB::KeyUp.IsDown())
	{
		Prev();
	}
	// 下キー押下？
	else if (KB::KeyDown.IsDown())
	{
		Next();
	}

	return 0;
}

int SystemWidgetPushView::Render(const SystemView* pView)
{
	m_label->Render(pView);
	return 0;
}

Rect SystemWidgetPushView::GetBox() const
{
	return m_label->GetBox();
}

void SystemWidgetPushView::SetPos(const PointF & pos)
{
	SystemWidget::SetPos(pos);
	m_label->SetPos(pos);
}





