#include "SystemWidgetSpin.hpp"
#include "Application.hpp"

std::shared_ptr<SystemWidgetSpin> SystemWidgetSpin::Create(const String& label, const std::function<void(int32_t)> cb)
{
	EQ_DURING
	{
		auto inst = std::shared_ptr<SystemWidgetSpin>(new SystemWidgetSpin);
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
		inst->m_label->SetPreset(String::ascii() + inst->leftArrow + inst->rightArrow + label);

		auto str = inst->MakeString();
		if (!inst->m_label->SetText(str))
		{
			EQ_THROW(u8"ラベルの作成に失敗しました。");
		}

		auto thiz = inst.get();
		inst->m_label->SetPostEffect([thiz](TextRenderer* renderer) {
			// 押している矢印の色を変える
			auto str32 = renderer->GetTextU32();
			if (!str32.empty())
			{
				auto left32 = thiz->leftArrow.to_u32().at(0);
				auto right32 = thiz->rightArrow.to_u32().at(0);
				int32_t count = 0;

				for (auto& c : str32)
				{
					bool blink = false;

					// 左が押されている？
					if (thiz->m_direction < 0 &&
						c == left32)
					{
						blink = true;
					}
					// 右が押されている？
					else if (thiz->m_direction > 0 &&
						c == right32)
					{
						blink = true;
					}

					if (blink)
					{
						// 色を変える
						std::shared_ptr<SpriteRenderer> sprite = renderer->GetLetter(count);
						sprite->SetColor(Color{ 255, 0, 0, 128 });
						break;
					}

					count++;
				}
			}
		});

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

	auto str = MakeString();
	m_label->SetText(str);
}

void SystemWidgetSpin::SetValue(int32_t val)
{
	m_val = val;

	auto str = MakeString();
	m_label->SetText(str);
}

void SystemWidgetSpin::SetFormatCallBack(const std::function<String(int32_t)>& cb)
{
	m_cbFormat = cb;

	auto str = MakeString();
	m_label->SetText(str);
}

void SystemWidgetSpin::OnEnter(const std::function<void(int32_t)>& cb)
{
	m_cbEnter = cb;
}

int32_t SystemWidgetSpin::GetValue() const
{
	return m_val;
}

void SystemWidgetSpin::SetCyclic(bool val)
{
	m_cyclic = val;
}

Rect SystemWidgetSpin::GetBox() const
{
	return m_label->GetBox();
}

const String SystemWidgetSpin::MakeString()
{
	String strVal = m_cbFormat ? m_cbFormat(m_val) : String::Sprintf(u8"%d", m_val);
	return m_text + "  " + leftArrow + " " + strVal + " " + rightArrow;
}

int SystemWidgetSpin::Do(SystemView* pView)
{
	int32_t valBak = m_val;

	m_label->SetPos(m_pos);

	m_direction = 0;

	// Enterキー押下？
	if (KB::KeyEnter.IsDown())
	{
		if (m_cbEnter)
		{
			m_cbEnter(m_val);
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
	// 左キー押下？
	else if (KB::KeyLeft.IsPress())
	{
		m_direction = -1;

		if(KB::KeyLeft.IsDown() || KB::KeyLeft.PressedDuration() > 300)
		{
			if (m_cyclic &&
				m_val == m_min)
			{
				m_val = m_max;
				m_cb(m_val);
			}
			else if (m_val > m_min)
			{
				// 値ダウン
				m_val = std::max(m_min, m_val - m_step);
				m_cb(m_val);
			}
		}
	}
	// 右キー押下？
	else if (KB::KeyRight.IsPress())
	{
		m_direction = 1;

		if (KB::KeyRight.IsDown() || KB::KeyRight.PressedDuration() > 300)
		{
			if (m_cyclic &&
				m_val == m_max)
			{
				m_val = m_min;
				m_cb(m_val);
			}
			else if (m_val < m_max)
			{
				// 値アップ
				m_val = std::min(m_max, m_val + m_step);
				m_cb(m_val);
			}
		}
	}

	if (valBak != m_val)
	{
		auto str = MakeString();
		m_label->SetText(str);
	}

	return 0;
}

void SystemWidgetSpin::SetPos(const PointF& pos)
{
	SystemWidget::SetPos(pos);
	m_label->SetPos(pos);
}

int SystemWidgetSpin::Render(const SystemView* pView)
{
	Size size = Window::Size();

	m_label->SetEnable(m_enable);
	m_label->Render(pView);

	return 0;
}

