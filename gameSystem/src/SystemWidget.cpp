#include "SystemWidget.hpp"

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



std::shared_ptr<SystemWidgetReturnView> SystemWidgetReturnView::Create(const String& label)
{
	EQ_DURING
	{
		auto inst = std::shared_ptr<SystemWidgetReturnView>(new SystemWidgetReturnView);
		if (!inst)
		{
			EQ_THROW(u8"インスタンスの作成に失敗しました。");
		}

		inst->m_label = label;
//		inst->m_cb = cb;

		return inst;
	}
	EQ_HANDLER
	{
		Logger::OutputError(EQ_GET_HANDLER().what());
	}
	EQ_END_HANDLER

	return nullptr;
}

int SystemWidgetReturnView::Do()
{
	// Zキー押下？
	if (KB::KeyZ.IsDown())
	{
	}

	return 0;
}

int SystemWidgetReturnView::Render()
{
	// label
	return 0;
}


std::shared_ptr<SystemWidgetEnterView> SystemWidgetEnterView::Create(const String& label)
{
	EQ_DURING
	{
		auto inst = std::shared_ptr<SystemWidgetEnterView>(new SystemWidgetEnterView);
		if (!inst)
		{
			EQ_THROW(u8"インスタンスの作成に失敗しました。");
		}

		inst->m_label = label;
//		inst->m_cb = cb;

		return inst;
	}
	EQ_HANDLER
	{
		Logger::OutputError(EQ_GET_HANDLER().what());
	}
	EQ_END_HANDLER

	return nullptr;
}

int SystemWidgetEnterView::Do()
{
	// Zキー押下？
	if (KB::KeyZ.IsDown())
	{
	}

	return 0;
}

int SystemWidgetEnterView::Render()
{
	// label
	return 0;
}

std::shared_ptr<SystemWidgetCustom> SystemWidgetCustom::Create(const String& label, const std::function<bool (void)> cb)
{
	EQ_DURING
	{
		auto inst = std::shared_ptr<SystemWidgetCustom>(new SystemWidgetCustom);
		if (!inst)
		{
			EQ_THROW(u8"インスタンスの作成に失敗しました。");
		}

		inst->m_label = label;
		inst->m_cb = cb;

		return inst;
	}
	EQ_HANDLER
	{
		Logger::OutputError(EQ_GET_HANDLER().what());
	}
	EQ_END_HANDLER

	return nullptr;
}

int SystemWidgetCustom::Do()
{
	if (m_exclusive)
	{
		m_exclusive = m_cb();
	}
	else
	{
		// Zキー押下？
		if (KB::KeyZ.IsDown())
		{
			m_exclusive = true;
		}
	}

	return 0;
}

int SystemWidgetCustom::Render()
{
	// label
	return 0;
}

std::shared_ptr<SystemWidgetSpin> SystemWidgetSpin::Create(const String& label, const std::function<void(int32_t)> cb)
{
	EQ_DURING
	{
		auto inst = std::shared_ptr<SystemWidgetSpin>(new SystemWidgetSpin);
		if (!inst)
		{
			EQ_THROW(u8"インスタンスの作成に失敗しました。");
		}

		inst->m_label = label;
		inst->m_cb = cb;

		return inst;
	}
	EQ_HANDLER
	{
		Logger::OutputError(EQ_GET_HANDLER().what());
	}
	EQ_END_HANDLER

	return nullptr;
}

void SystemWidgetSpin::SetRange(int32_t min, int32_t max, int32_t step)
{
	m_min = min;
	m_max = max;
	m_step = step;
	m_val = 0;
}

void SystemWidgetSpin::SetValue(int32_t val)
{
	m_val = val;
}

int32_t SystemWidgetSpin::GetValue() const
{
	return m_val;
}

int SystemWidgetSpin::Do()
{
	// 上キー押下？
	if (KB::KeyUp.IsDown())
	{
		Prev();
	}
	// 下キー押下？
	else if (KB::KeyDown.IsDown())
	{
		Next();
	}
	// 左キー押下？
	else if (KB::KeyLeft.IsDown())
	{
		if (m_val > m_min)
		{
			// 値ダウン
			m_val = std::max(m_min, m_val - m_step);
			m_cb(m_val);
		}
	}
	// 右キー押下？
	else if (KB::KeyLeft.IsDown())
	{
		if (m_val < m_max)
		{
			// 値アップ
			m_val = std::min(m_max, m_val + m_step);
			m_cb(m_val);
		}
	}

	return 0;
}

int SystemWidgetSpin::Render()
{
	// label < val >
	return 0;
}
