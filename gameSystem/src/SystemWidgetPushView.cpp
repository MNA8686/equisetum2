#include "SystemWidgetPushView.hpp"
#include "Application.hpp"


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





