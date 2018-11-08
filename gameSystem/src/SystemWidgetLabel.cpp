#include "SystemWidgetLabel.hpp"
#include "Application.hpp"

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

bool SystemWidgetLabel::SetPreset(std::shared_ptr<BitmapFont> bitmapFont)
{
	m_renderer->SetBitmapFont(bitmapFont);
	return true;
}

bool SystemWidgetLabel::SetText(const String & label)
{
	m_renderer->SetText(label);
	return true;
}

int SystemWidgetLabel::Do(SystemView* pView)
{
	return 0;
}

std::shared_ptr<TextRenderer>& SystemWidgetLabel::GetRenderer()
{
	return m_renderer;
}

int SystemWidgetLabel::Render(const SystemView* pView)
{
	m_renderer->SetLayer(m_layer);

	// 座標を設定する
	PointF parentPos;
	if (pView)
	{
		parentPos.x = pView->GetPos().x;
		parentPos.y = pView->GetPos().y;
	}

	Size size = Window::Size();
	m_renderer->SetPos({ static_cast<int32_t>(size.x * (parentPos.x + m_pos.x)), static_cast<int32_t>(size.y * (parentPos.y + m_pos.y)) });

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

Rect SystemWidgetLabel::GetBox() const
{
	const Size boxSize = m_renderer->GetBoxSize();
	Size windowSize = Window::Size();

	return Rect{ static_cast<int32_t>(windowSize.x * m_pos.x), static_cast<int32_t>(windowSize.y * m_pos.y) - boxSize.y / 2, boxSize.x, boxSize.y };
}
