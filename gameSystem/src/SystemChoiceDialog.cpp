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
		const float textSize = 0.1f;
	
		// 外枠
		{
			// サイズを求める
			SizeF margin = { 0.05f, 0.05f };
			Rect rectBorder{
				static_cast<int32_t>(windowSize.x * margin.x),
				static_cast<int32_t>(windowSize.y * margin.y),
				static_cast<int32_t>(windowSize.x * (1.f - margin.x * 2)),
				static_cast<int32_t>(windowSize.y * (1.f - margin.y * 2))
			};
			inst->m_rectBorder = rectBorder;
			
			inst->m_rectRendererBorder = std::dynamic_pointer_cast<RectRenderer>(GetApplication()->GetRenderer()->CreateRenderObject(RenderType::PRIMITIVE, PrimitiveType::RECT));
			inst->m_rectRendererBorder->SetBlendMode(BlendMode::Blend);
			inst->m_rectRendererBorder->SetColor(Color{ 0, 180, 0, 200 });
			inst->m_rectRendererBorder->SetRect(inst->m_rectBorder, true);
		}

		// 内枠
		{
			// サイズを求める
			SizeF margin = { 0.04f, 0.04f };
			Rect rectInnerFrame{
				inst->m_rectBorder.x + static_cast<int32_t>(windowSize.x * margin.x),
				inst->m_rectBorder.y + static_cast<int32_t>(windowSize.y * textSize),
				inst->m_rectBorder.width - static_cast<int32_t>(windowSize.x * margin.x) * 2,
				inst->m_rectBorder.height - static_cast<int32_t>(windowSize.y * (margin.y + textSize))
			};
			inst->m_rectInnerFrame = rectInnerFrame;
			
			inst->m_rectRendererInnerFrame = std::dynamic_pointer_cast<RectRenderer>(GetApplication()->GetRenderer()->CreateRenderObject(RenderType::PRIMITIVE, PrimitiveType::RECT));
			inst->m_rectRendererInnerFrame->SetBlendMode(BlendMode::Blend);
			inst->m_rectRendererInnerFrame->SetColor(Color{ 32, 200, 32, 200 });
			inst->m_rectRendererInnerFrame->SetRect(inst->m_rectInnerFrame, true);
		}

		// タイトル設定
		inst->m_label->SetPivot({ 0.5f, 0.5f });
		inst->m_label->SetPos({ (inst->m_rectBorder.x + inst->m_rectBorder.width / 2) / static_cast<float>(windowSize.x),
								inst->m_rectBorder.y / static_cast<float>(windowSize.y) + textSize / 2.f });

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

