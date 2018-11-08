#include "SystemChoiceDialog.hpp"
#include "Application.hpp"

std::shared_ptr<SystemChoiceDialog> SystemChoiceDialog::Create(const String & title, const std::vector<String>& vItem, const std::function<void(int index, const String&item)> cb)
{
	EQ_DURING
	{
		if (!cb)
		{
			EQ_THROW(u8"コールバックが設定されていません。");
		}

		auto inst = std::make_shared<SystemChoiceDialog>();
		if (!inst)
		{
			EQ_THROW(u8"インスタンスの作成に失敗しました。");
		}

		inst->m_cb = cb;
		inst->m_title = title;
		inst->m_vItem = vItem;
		inst->m_isActive = true;

		inst->m_label = SystemWidgetLabel::Create(title);
		if (!inst->m_label)
		{
			EQ_THROW(u8"ラベルの作成に失敗しました。");
		}
		inst->m_label->SetPreset(u8" " + title);

		if (!inst->m_label->SetText(title))
		{
			EQ_THROW(u8"ラベルの作成に失敗しました。");
		}

		Size windowSize = Window::Size();
	
		// 外枠
		{
			// ピクセルサイズを求める
			Rect rectBorderPixel{
				static_cast<int32_t>(windowSize.x * inst->m_rectBorder.x),
				static_cast<int32_t>(windowSize.y * inst->m_rectBorder.y),
				static_cast<int32_t>(windowSize.x * inst->m_rectBorder.width),
				static_cast<int32_t>(windowSize.y * inst->m_rectBorder.height)
			};
			inst->m_rectBorderPixel = rectBorderPixel;
			
			inst->m_rectRendererBorder = std::dynamic_pointer_cast<RectRenderer>(GetApplication()->GetRenderer()->CreateRenderObject(RenderType::PRIMITIVE, PrimitiveType::RECT));
			inst->m_rectRendererBorder->SetBlendMode(BlendMode::Blend);
			inst->m_rectRendererBorder->SetColor(Color{ 0, 180, 0, 200 });
			inst->m_rectRendererBorder->SetRect(inst->m_rectBorderPixel, true);
		}

		// 内枠
		{
			// ピクセルサイズを求める
			Rect rectInnerFramePixel{
				static_cast<int32_t>(windowSize.x * inst->m_rectInnerFrame.x),
				static_cast<int32_t>(windowSize.y * inst->m_rectInnerFrame.y),
				static_cast<int32_t>(windowSize.x * inst->m_rectInnerFrame.width),
				static_cast<int32_t>(windowSize.y * inst->m_rectInnerFrame.height)
			};
			inst->m_rectInnerFramePixel = rectInnerFramePixel;
			
			inst->m_rectRendererInnerFrame = std::dynamic_pointer_cast<RectRenderer>(GetApplication()->GetRenderer()->CreateRenderObject(RenderType::PRIMITIVE, PrimitiveType::RECT));
			inst->m_rectRendererInnerFrame->SetBlendMode(BlendMode::Blend);
			inst->m_rectRendererInnerFrame->SetColor(Color{ 32, 200, 32, 200 });
			inst->m_rectRendererInnerFrame->SetRect(inst->m_rectInnerFramePixel, true);
		}

		inst->m_maxRow = 5;

		// タイトル設定
		inst->m_label->SetPivot({ 0.5f, 0.5f });
		inst->m_label->SetPos({ inst->m_rectBorder.x + inst->m_rectBorder.width / 2,
								inst->m_rectBorder.y + inst->textMargin / 2.f });

		// 共通のフォントデータを作る
		{
			String commonStr;
			for (auto& str : vItem)
			{
				commonStr += str;
			}

			std::shared_ptr<BitmapFont> bitmapFont = GetApplication()->GetSystemFont()->MakeBitmapFont(commonStr, Color{ 0xff, 0xff, 0xff, 0xff });
			if (!bitmapFont)
			{
				EQ_THROW(u8"ビットマップフォントの作成に失敗しました。");
			}
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

int SystemChoiceDialog::Do(SystemView * pView)
{
	if(!m_isActive)
	{
		return -1;
	}

	return 0;
}

int SystemChoiceDialog::Render(const SystemView * pView)
{
	if(!m_isActive)
	{
		return -1;
	}

	GetApplication()->GetRenderer()->AddRenderQueue(m_rectRendererBorder.get());
	GetApplication()->GetRenderer()->AddRenderQueue(m_rectRendererInnerFrame.get());

	m_label->Render(pView);

	return 0;
}

bool SystemChoiceDialog::IsActive() const
{
	return m_isActive;
}

