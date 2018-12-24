#include "SystemWidgetLabelWithFont.hpp"
#include "Application.hpp"

std::shared_ptr<SystemWidgetLabelWithFont> SystemWidgetLabelWithFont::CreateWithFont(const String& label, std::shared_ptr<FontManager> font)
{
	EQ_DURING
	{
		auto inst = std::shared_ptr<SystemWidgetLabelWithFont>(new SystemWidgetLabelWithFont);
		if (!inst)
		{
			EQ_THROW(u8"インスタンスの作成に失敗しました。");
		}

		if (!inst->SetPresetWithFont(label, font))
		{
			EQ_THROW(u8"ビットマップフォントの設定に失敗しました。");
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

std::shared_ptr<SystemWidgetLabelWithFont> SystemWidgetLabelWithFont::CreateWithBitmapFont(const String& label, std::shared_ptr<BitmapFont> bitmapFont)
{
	EQ_DURING
	{
		auto inst = std::shared_ptr<SystemWidgetLabelWithFont>(new SystemWidgetLabelWithFont);
		if (!inst)
		{
			EQ_THROW(u8"インスタンスの作成に失敗しました。");
		}

		if (!inst->SetBitmapFont(bitmapFont))
		{
			EQ_THROW(u8"ビットマップフォントの設定に失敗しました。");
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

SystemWidgetLabelWithFont::SystemWidgetLabelWithFont()
{
	m_renderer = GetApplication()->GetRenderer()->CreateRenderObject<TextRenderer>();
	m_renderer->SetPivot({ 0, 0.5f });
	m_renderer->SetBlendMode(BlendMode::Blend);
}

bool SystemWidgetLabelWithFont::SetPresetWithFont(const String& preset, const std::shared_ptr<FontManager> fontManager)
{
	EQ_DURING
	{
		if (!fontManager)
		{
			EQ_THROW(u8"フォントが設定されていません。");
		}

		m_preset = preset;
		std::shared_ptr<BitmapFont> bitmapFont = fontManager->MakeBitmapFont(m_preset, Color{ 0xff, 0xff, 0xff, 0xff });
		if (!bitmapFont)
		{
			EQ_THROW(u8"ビットマップフォントの作成に失敗しました。");
		}

		SetBitmapFont(bitmapFont);

		return true;
	}
	EQ_HANDLER
	{
		Logger::OutputError(EQ_GET_HANDLER().what());
	}
	EQ_END_HANDLER

	return false;
}

bool SystemWidgetLabelWithFont::SetBitmapFont(std::shared_ptr<BitmapFont> bitmapFont)
{
	m_renderer->SetBitmapFont(bitmapFont);
	return true;
}

bool SystemWidgetLabelWithFont::SetText(const String & label)
{
	m_renderer->SetText(label);
	return true;
}

int SystemWidgetLabelWithFont::Do(SystemView* pView)
{
	return 0;
}

std::shared_ptr<TextRenderer>& SystemWidgetLabelWithFont::GetRenderer()
{
	return m_renderer;
}

int SystemWidgetLabelWithFont::Render(const SystemView* pView)
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
	auto color = m_enable ? Color{ 5, 218, 20, 128 } : Color{ 5, 32, 20, 128 };
	m_renderer->SetColor(color);

	// ポストエフェクトを呼び出す
	if (m_cb)
	{
		m_cb(m_renderer.get());
	}

	GetApplication()->GetRenderer()->AddRenderQueue(m_renderer.get());

	return false;
}

Size SystemWidgetLabelWithFont::GetBoxSize() const
{
	return m_renderer->GetBoxSize();
}

void SystemWidgetLabelWithFont::SetPivot(PointF pivot)
{
	m_renderer->SetPivot(pivot);
}

void SystemWidgetLabelWithFont::SetPostEffect(const std::function<void(TextRenderer*)> cb)
{
	m_cb = cb;
}

Rect SystemWidgetLabelWithFont::GetBox() const
{
	const Size boxSize = m_renderer->GetBoxSize();
	Size windowSize = Window::Size();

	return Rect{ static_cast<int32_t>(windowSize.x * m_pos.x), static_cast<int32_t>(windowSize.y * m_pos.y) - boxSize.y / 2, boxSize.x, boxSize.y };
}

