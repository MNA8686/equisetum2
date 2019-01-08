#include "SystemWidgetCustom.hpp"
#include "Application.hpp"

std::shared_ptr<SystemWidgetCustom> SystemWidgetCustom::Create(const String& label, const std::function<bool (void)> cb)
{
	EQ_DURING
	{
		auto inst = std::shared_ptr<SystemWidgetCustom>(new SystemWidgetCustom);
		if (!inst)
		{
			EQ_THROW(u8"インスタンスの作成に失敗しました。");
		}

		inst->m_text = label;
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

int SystemWidgetCustom::Do(SystemView* pView)
{
	m_label->SetPos(m_pos);

	if (m_exclusive)
	{
		m_exclusive = m_cb();
		if (m_exclusive)
		{
			m_stat = Stat::Exclusive;
		}
	}
	else
	{
		// Enterキー押下？
		if (KB::KeyEnter.IsPress())
		{
			if (KB::KeyEnter.IsDown())
			{
				m_exclusive = true;
			}
		
			m_label->SetPos(m_pos + m_addPosOnPuress);
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
	}

	return 0;
}

int SystemWidgetCustom::Render(const SystemView* pView)
{
	// label
	Size size = Window::Size();

	m_label->SetEnable(m_enable);
	m_label->Render(pView);

	return 0;
}

Rect SystemWidgetCustom::GetBox() const
{
	return m_label->GetBox();
}

void SystemWidgetCustom::SetPos(const PointF & pos)
{
	SystemWidget::SetPos(pos);
	m_label->SetPos(pos);
}

