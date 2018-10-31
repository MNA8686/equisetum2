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

void SystemWidget::SetEnable(bool enable)
{
	m_enable = enable;
}

std::shared_ptr<TextRenderer>& SystemWidget::GetLabelRenderer()
{
	return m_labelRenderer;
}

bool SystemWidget::SetLabel(const String& label)
{
	EQ_DURING
	{
		if (!m_labelRenderer)
		{
			m_labelRenderer = std::dynamic_pointer_cast<TextRenderer>(GetApplication()->GetRenderer()->CreateRenderObject(RenderType::TEXT));
			if (!m_labelRenderer)
			{
				EQ_THROW(u8"レンダラの作成に失敗しました。");
			}
		}

		std::shared_ptr<BitmapFont> bitmapFont = GetApplication()->GetSystemFont()->MakeBitmapFont(u8" <0123456789>" + label, Color{0xff, 0xff, 0xff, 0xff});
		if (!bitmapFont)
		{
			EQ_THROW(u8"ビットマップフォントの作成に失敗しました。");
		}

		m_labelRenderer->SetBitmapFont(bitmapFont);
		m_labelRenderer->SetText(label);
		m_labelRenderer->SetPivot({ 0, 0.5f });
		m_labelRenderer->SetBlendMode(BlendMode::Blend);

		return true;
	}
	EQ_HANDLER
	{
		Logger::OutputError(EQ_GET_HANDLER().what());
	}
	EQ_END_HANDLER

	return false;
}

bool SystemWidget::RenderLabel()
{
	Size size = Window::Size();
	m_labelRenderer->SetPos({ static_cast<int32_t>(size.x * m_pos.x), static_cast<int32_t>(size.y * m_pos.y) });

	uint8_t color = m_enable ? 128 : 96;
	m_labelRenderer->SetColor(Color{ color, color, color, 128 });

	RenderLabelPostEffect();

	GetApplication()->GetRenderer()->AddRenderQueue(m_labelRenderer.get());

	return true;
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
	// Enterキー押下？
	if (KB::KeyEnter.IsDown())
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
		if (!inst->SetLabel(label))
		{
			EQ_THROW(u8"ラベルの作成に失敗しました。");
		}

		inst->m_rectRenderer = std::dynamic_pointer_cast<RectRenderer>(GetApplication()->GetRenderer()->CreateRenderObject(RenderType::PRIMITIVE, PrimitiveType::RECT));
		if (!inst->m_rectRenderer)
		{
			EQ_THROW(u8"レンダラの作成に失敗しました。");
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

int SystemWidgetCustom::Do()
{
	if (m_exclusive)
	{
		m_stat = Stat::Exclusive;
		m_exclusive = m_cb();
	}
	else
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
		// Zキー押下？
		else if (KB::KeyZ.IsDown())
		{
			m_exclusive = true;
		}
	}

	return 0;
}

int SystemWidgetCustom::Render()
{
	// label
	Size size = Window::Size();
	auto label = GetLabelRenderer();

#if 0
	const String text = String::Sprintf(u8"%s", m_label.c_str());
	label->SetColor(m_enable ? Color{128, 128, 128, 128} : Color{96, 96, 96, 128});
	label->SetText(text);
	label->SetPivot({ 0, 0.5f });
	label->SetPos({ static_cast<int32_t>(size.x * m_pos.x), static_cast<int32_t>(size.y * m_pos.y) });
#endif

	if (GetFocus())
	{
		m_rectRenderer->SetVisible(true);
		m_rectRenderer->SetColor(Color{ 255, 255, 255, 100 });
		m_rectRenderer->SetBlendMode(BlendMode::Blend);
		Size boxSize = label->GetBoxSize();
		m_rectRenderer->SetRect(Rect{ static_cast<int32_t>(size.x * m_pos.x), static_cast<int32_t>(size.y * m_pos.y) - boxSize.y / 2, boxSize.x, boxSize.y });
	
		GetApplication()->GetRenderer()->AddRenderQueue(m_rectRenderer.get());
	}

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
		auto str = inst->MakeString();
		if (!inst->SetLabel(label))
		{
			EQ_THROW(u8"ラベルの作成に失敗しました。");
		}

		inst->m_rectRenderer = std::dynamic_pointer_cast<RectRenderer>(GetApplication()->GetRenderer()->CreateRenderObject(RenderType::PRIMITIVE, PrimitiveType::RECT));
		if (!inst->m_rectRenderer)
		{
			EQ_THROW(u8"レンダラの作成に失敗しました。");
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

void SystemWidgetSpin::SetRange(int32_t min, int32_t max, int32_t step)
{
	m_min = min;
	m_max = max;
	m_step = step;
	m_val = 0;

	auto str = MakeString();
	SetLabel(str);
}

void SystemWidgetSpin::SetValue(int32_t val)
{
	m_val = val;

	auto str = MakeString();
	SetLabel(str);
}

int32_t SystemWidgetSpin::GetValue() const
{
	return m_val;
}

void SystemWidgetSpin::SetCyclic(bool val)
{
	m_cyclic = val;
}

void SystemWidgetSpin::RenderLabelPostEffect()
{
	// 押している矢印の色を変える
	const String left = u8"◀";
	const String right = u8"▶";

	auto label = GetLabelRenderer();

	auto str32 = label->GetTextU32();
	if (!str32.empty())
	{
		auto left32 = left.to_u32().at(0);
		auto right32 = right.to_u32().at(0);
		int32_t count = 0;
		for (auto& c : str32)
		{
			bool blink = false;

			if (m_direction < 0 &&
				c == left32)
			{
				blink = true;
			}
			else if (m_direction > 0 &&
				c == right32)
			{
				blink = true;
			}

			if (blink)
			{
				std::shared_ptr<SpriteRenderer> sprite = label->GetLetter(count);
				sprite->SetColor(Color{ 255, 0, 0, 128 });
				break;
			}

			count++;
		}
	}
}

const String SystemWidgetSpin::MakeString()
{
	const String left = u8"◀";
	const String right = u8"▶";

	const String text = String::Sprintf(u8"%s  %s %d %s", m_label.c_str(), left.c_str(), m_val, right.c_str());
	return text;
}

int SystemWidgetSpin::Do()
{
	int32_t valBak = m_val;

	m_direction = 0;

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
		SetLabel(str);
	}

	return 0;
}

int SystemWidgetSpin::Render()
{
	Size size = Window::Size();
	const String left = u8"◀";
	const String right = u8"▶";

	auto label = GetLabelRenderer();

	if (GetFocus())
	{
		m_rectRenderer->SetVisible(true);
		m_rectRenderer->SetColor(Color{ 255, 255, 255, 100 });
		m_rectRenderer->SetBlendMode(BlendMode::Blend);
		Size boxSize = label->GetBoxSize();
		m_rectRenderer->SetRect(Rect{ static_cast<int32_t>(size.x * m_pos.x), static_cast<int32_t>(size.y * m_pos.y) - boxSize.y / 2, boxSize.x, boxSize.y });
	
		GetApplication()->GetRenderer()->AddRenderQueue(m_rectRenderer.get());
	}

	return 0;
}
