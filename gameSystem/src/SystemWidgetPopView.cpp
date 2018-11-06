#include "SystemWidgetPopView.hpp"
#include "Application.hpp"


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


