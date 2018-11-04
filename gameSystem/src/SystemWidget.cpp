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

Rect SystemWidget::GetBox() const
{
	const Size boxSize = m_label->GetBoxSize();
	Size windowSize = Window::Size();

	return Rect{ static_cast<int32_t>(windowSize.x * m_pos.x), static_cast<int32_t>(windowSize.y * m_pos.y) - boxSize.y / 2, boxSize.x, boxSize.y };
}




std::shared_ptr<SystemWidgetMenu> SystemWidgetMenu::Create(const String& label)
{
	auto p = std::make_shared<SystemWidgetMenu>();

	p->m_rectRenderer = std::dynamic_pointer_cast<RectRenderer>(GetApplication()->GetRenderer()->CreateRenderObject(RenderType::PRIMITIVE, PrimitiveType::RECT));
	if (!p->m_rectRenderer)
	{
	//	EQ_THROW(u8"レンダラの作成に失敗しました。");
	}

	return p;
}

int SystemWidgetMenu::SetWidget(std::shared_ptr<SystemWidget> pWidget)
{
	m_vWidget.push_back(pWidget);


	pWidget->SetPos({ m_pos.x, m_pos.y + (m_vWidget.size() - 1) * 0.05f });

	return 0;
}

int SystemWidgetMenu::Do()
{
	size_t index = 0;
	size_t nextFocus = -1;
	bool enable = true;

	for (auto& widget : m_vWidget)
	{
		if (widget->GetFocus())
		{
			widget->Prepare();
			widget->Do();

			m_cursorNow = widget->GetBox();

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

			// メニュー項目を移動した？
			if (index != nextFocus)
			{
				widget->SetFocus(false);
				m_vWidget[nextFocus]->SetFocus(true);

				m_cursorSrc = m_cursorNow;
				m_cursorDest = m_vWidget[nextFocus]->GetBox();
				m_cursolMoveCounter = 1;
			}

			// 排他操作を行うウィジェットに入ったらメニュー内の項目を無効にする
			if (widget->GetStat() == SystemWidget::Stat::Exclusive)
			{
				enable = false;
			}
			break;
		}

		index++;
	}

	index = 0;
	for (auto& widget : m_vWidget)
	{
		widget->SetPos({ m_pos.x, m_pos.y + index * 0.05f });
		widget->SetEnable(enable);
		index++;
	}

	// 何フレーム掛けてカーソルを移動させるか
	int32_t maxFrame = 8;
	// カーソルアニメーション中？
	if(m_cursolMoveCounter > 0 && m_cursolMoveCounter < maxFrame)
	{
		if(m_cursolMoveCounter == maxFrame)
		{
			// カーソルアニメーション終了
			m_cursorNow = m_cursorDest;
			m_cursolMoveCounter = 0;
		}
		else
		{
			// 1フレームあたりのカーソル移動量を算出
			float maxFrameF = static_cast<float>(maxFrame);
			auto subXPerFrame = (m_cursorDest.x - m_cursorSrc.x) / maxFrameF;
			auto subYPerFrame = (m_cursorDest.y - m_cursorSrc.y) / maxFrameF;
			auto subWidthPerFrame = (m_cursorDest.width - m_cursorSrc.width) / maxFrameF;
			auto subHeightPerFrame = (m_cursorDest.height - m_cursorSrc.height) / maxFrameF;
			
			// 現在のカーソル位置を算出
			m_cursorNow = {
				m_cursorSrc.x + static_cast<int32_t>(subXPerFrame * m_cursolMoveCounter),
				m_cursorSrc.y + static_cast<int32_t>(subYPerFrame * m_cursolMoveCounter),
				m_cursorSrc.width + static_cast<int32_t>(subWidthPerFrame * m_cursolMoveCounter),
				m_cursorSrc.height + static_cast<int32_t>(subHeightPerFrame * m_cursolMoveCounter),
			};

			// カーソルアニメーション進行
			m_cursolMoveCounter++;
		}
	}

	return 0;
}

int SystemWidgetMenu::Render()
{
	// カーソルの枠を描画
	m_rectRenderer->SetRect(m_cursorNow);
	m_rectRenderer->SetColor(Color{ 255, 255, 255, 100 });
	m_rectRenderer->SetBlendMode(BlendMode::Blend);
	GetApplication()->GetRenderer()->AddRenderQueue(m_rectRenderer.get());

	// このメニュー内のウィジェットを描画
	for (auto& widget : m_vWidget)
	{
		widget->Render();
	}

	return 0;
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

		inst->m_text = label;

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
	m_label->Render(this);
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

		inst->m_text = label;

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
	m_label->Render(this);
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

		inst->m_text = label;
		inst->m_cb = cb;
		inst->m_label = std::make_shared<Label>();
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

	m_label->Render(this);

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

		inst->m_text = label;
		inst->m_cb = cb;
		inst->m_label = std::make_shared<LabelEx>();
		inst->m_label->SetPreset(u8" 0123456789-+" + inst->leftArrow + inst->rightArrow + label);

		auto str = inst->MakeString();
		if (!inst->m_label->SetText(str))
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

int32_t SystemWidgetSpin::GetValue() const
{
	return m_val;
}

void SystemWidgetSpin::SetCyclic(bool val)
{
	m_cyclic = val;
}

const String SystemWidgetSpin::MakeString()
{
	const String text = String::Sprintf(u8"%s  %s %d %s", m_text.c_str(), leftArrow.c_str(), m_val, rightArrow.c_str());
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
		m_label->SetText(str);
	}

	return 0;
}

int SystemWidgetSpin::Render()
{
	Size size = Window::Size();

	m_label->Render(this);

	return 0;
}

SystemWidget::Label::Label()
{
	m_renderer = std::dynamic_pointer_cast<TextRenderer>(GetApplication()->GetRenderer()->CreateRenderObject(RenderType::TEXT));
	m_renderer->SetPivot({ 0, 0.5f });
	m_renderer->SetBlendMode(BlendMode::Blend);
}

bool SystemWidget::Label::Render(SystemWidget * pWidget)
{
	// 座標を設定する
	Size size = Window::Size();
	m_renderer->SetPos({ static_cast<int32_t>(size.x * pWidget->m_pos.x), static_cast<int32_t>(size.y * pWidget->m_pos.y) });

	// 有効か無効かで文字の色を変える
	uint8_t color = pWidget->m_enable ? 128 : 96;
	m_renderer->SetColor(Color{ color, color, color, 128 });

	// ポストエフェクトを呼び出す
	RenderPostEffect(pWidget);

	GetApplication()->GetRenderer()->AddRenderQueue(m_renderer.get());

	return true;
}

Size SystemWidget::Label::GetBoxSize() const
{
	return m_renderer->GetBoxSize();
}

std::shared_ptr<TextRenderer>& SystemWidget::Label::GetRenderer()
{
	return m_renderer;
}

bool SystemWidget::Label::SetPreset(const String & preset)
{
	EQ_DURING
	{
		std::shared_ptr<BitmapFont> bitmapFont = GetApplication()->GetSystemFont()->MakeBitmapFont(preset, Color{0xff, 0xff, 0xff, 0xff});
		if (!bitmapFont)
		{
			EQ_THROW(u8"ビットマップフォントの作成に失敗しました。");
		}

		m_renderer->SetBitmapFont(bitmapFont);

		return true;
	}
	EQ_HANDLER
	{
		Logger::OutputError(EQ_GET_HANDLER().what());
	}
	EQ_END_HANDLER

	return false;
}

bool SystemWidget::Label::SetText(const String & label)
{
	m_renderer->SetText(label);
	return true;
}

void SystemWidgetSpin::LabelEx::RenderPostEffect(SystemWidget * pWidget)
{
	// 押している矢印の色を変える
	auto renderer = GetRenderer();
	auto pWidget_ = static_cast<SystemWidgetSpin*>(pWidget);

	auto str32 = renderer->GetTextU32();
	if (!str32.empty())
	{
		auto left32 = pWidget_->leftArrow.to_u32().at(0);
		auto right32 = pWidget_->rightArrow.to_u32().at(0);
		int32_t count = 0;
		
		for (auto& c : str32)
		{
			bool blink = false;

			// 左が押されている？
			if (pWidget_->m_direction < 0 &&
				c == left32)
			{
				blink = true;
			}
			// 右が押されている？
			else if (pWidget_->m_direction > 0 &&
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
}

std::shared_ptr<SystemWidgetLabel> SystemWidgetLabel::Create(const String & label)
{
	EQ_DURING
	{
		auto inst = std::shared_ptr<SystemWidgetLabel>(new SystemWidgetLabel);
		if (!inst)
		{
			EQ_THROW(u8"インスタンスの作成に失敗しました。");
		}

		if (!inst->SetPreset(label))
		{
			EQ_THROW(u8"プリセットの作成に失敗しました。");
		}
		if (!inst->SetText(label))
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

SystemWidgetLabel::SystemWidgetLabel()
{
	m_renderer = std::dynamic_pointer_cast<TextRenderer>(GetApplication()->GetRenderer()->CreateRenderObject(RenderType::TEXT));
	m_renderer->SetPivot({ 0, 0.5f });
	m_renderer->SetBlendMode(BlendMode::Blend);
}

bool SystemWidgetLabel::SetPreset(const String & preset)
{
	EQ_DURING
	{
		std::shared_ptr<BitmapFont> bitmapFont = GetApplication()->GetSystemFont()->MakeBitmapFont(preset, Color{0xff, 0xff, 0xff, 0xff});
		if (!bitmapFont)
		{
			EQ_THROW(u8"ビットマップフォントの作成に失敗しました。");
		}

		m_renderer->SetBitmapFont(bitmapFont);

		return true;
	}
	EQ_HANDLER
	{
		Logger::OutputError(EQ_GET_HANDLER().what());
	}
	EQ_END_HANDLER

	return false;
}

bool SystemWidgetLabel::SetText(const String & label)
{
	m_renderer->SetText(label);
	return true;
}

int SystemWidgetLabel::Do()
{
	return 0;
}

int SystemWidgetLabel::Render()
{
	// 座標を設定する
	Size size = Window::Size();
	m_renderer->SetPos({ static_cast<int32_t>(size.x * m_pos.x), static_cast<int32_t>(size.y * m_pos.y) });

	// 有効か無効かで文字の色を変える
	uint8_t color = m_enable ? 128 : 96;
	m_renderer->SetColor(Color{ color, color, color, 128 });

	// ポストエフェクトを呼び出す
	if (m_cb)
	{
		m_cb(m_renderer.get());
	}

	GetApplication()->GetRenderer()->AddRenderQueue(m_renderer.get());

	return false;
}

Size SystemWidgetLabel::GetBoxSize() const
{
	return m_renderer->GetBoxSize();
}

void SystemWidgetLabel::SetPivot(PointF pivot)
{
	m_renderer->SetPivot(pivot);
}

void SystemWidgetLabel::SetPostEffect(const std::function<void(TextRenderer*)> cb)
{
	m_cb = cb;
}

std::shared_ptr<TextRenderer>& SystemWidgetLabel::GetRenderer()
{
	return m_renderer;
}

