#include "SystemChoiceDialog.hpp"
#include "Application.hpp"

std::shared_ptr<SystemChoiceDialog> SystemChoiceDialog::Create(const String & title, const std::vector<String>& vItem, const std::function<void(int index, const String&item)> cb)
{
	int layer = 1;

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
		inst->m_label->Setlayer(layer);

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
			
			inst->m_rectRendererBorder = GetApplication()->GetRenderer()->CreateRenderObject<RectRenderer>();
			inst->m_rectRendererBorder->SetBlendMode(BlendMode::Blend);
			inst->m_rectRendererBorder->SetColor(Color{ 0, 180, 0, 200 });
			inst->m_rectRendererBorder->SetRect(inst->m_rectBorderPixel, true);
			inst->m_rectRendererBorder->SetLayer(layer);
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
			
			inst->m_rectRendererInnerFrame = GetApplication()->GetRenderer()->CreateRenderObject<RectRenderer>();
			inst->m_rectRendererInnerFrame->SetBlendMode(BlendMode::Blend);
			inst->m_rectRendererInnerFrame->SetColor(Color{ 32, 200, 32, 200 });
			inst->m_rectRendererInnerFrame->SetRect(inst->m_rectInnerFramePixel, true);
			inst->m_rectRendererInnerFrame->SetLayer(layer);
		}

		// タイトル設定
		inst->m_label->SetPivot({ 0.5f, 0.5f });
		inst->m_label->SetPos({ inst->m_rectBorder.x + inst->m_rectBorder.width / 2,
								inst->m_rectBorder.y + inst->textMargin / 2.f });

		// アイテム表示欄作成
		{
			// 共通のフォントを作成する
			String commonStr = u8" 0123456789[]:";
			for (auto& str : vItem)
			{
				commonStr += str;
			}

			std::shared_ptr<BitmapFont> bitmapFont = GetApplication()->GetSystemFont()->MakeBitmapFont(commonStr, Color{ 0xff, 0xff, 0xff, 0xff });
			if (!bitmapFont)
			{
				EQ_THROW(u8"ビットマップフォントの作成に失敗しました。");
			}

			// 最大行数算出
			inst->m_maxRow = static_cast<int32_t>(inst->m_rectInnerFrame.height / inst->textSize);
			
			// 行数分だけラベルを作成する
			for (int32_t count = 0; count < inst->m_maxRow; count++)
			{
				auto label = SystemWidgetLabel::Create(u8" ");
				if (!label)
				{
					EQ_THROW(u8"ラベルの作成に失敗しました。");
				}
				label->SetPreset(bitmapFont);
				label->SetPivot({ 0, 0 });
				label->SetPos({inst->m_rectInnerFrame.x + 0.01f, inst->m_rectInnerFrame.y + count * inst->textSize});
				label->Setlayer(layer);

				inst->m_vLabel.push_back(label);
			}
		}

		if (vItem.size() > 0)
		{
			inst->m_cursor = 0;
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

	auto size = static_cast<int32_t>(m_vItem.size());

	// 上キー押下？
	if (KB::KeyUp.IsPress())
	{
		if (KB::KeyUp.IsDown() || KB::KeyUp.PressedDuration() > 300)
		{
			if (m_cursor > 0)
			{
				m_cursor--;
			}

			// 表示開始位置をずらす
			if (m_cursor < m_index)
			{
				m_index--;
			}
		}
	}
	// 下キー押下？
	else if (KB::KeyDown.IsPress())
	{
		if (KB::KeyDown.IsDown() || KB::KeyDown.PressedDuration() > 300)
		{
			if (size != 0)
			{
				m_cursor++;
				if (m_cursor >= size)
				{
					m_cursor = size - 1;
				}

				// 表示開始位置をずらす
				if (m_index + m_maxRow <= m_cursor)
				{
					m_index++;
				}
			}
		}
	}
	else if (KB::KeyEnter.IsDown())
	{
		m_cb(m_cursor, m_vItem[m_cursor]);
		m_isActive = false;
	}
	else if (KB::KeyEscape.IsDown())
	{
		m_cb(-1, "");
		m_isActive = false;
	}

	// アイテムを表示する
	for (int32_t count = 0; count < m_maxRow; count++)
	{
		if (m_index + count < size)
		{
			m_vLabel[count]->SetText(String::Sprintf(u8"[%d] ", m_index + count + 1) + m_vItem[m_index + count]);
			m_vLabel[count]->SetEnable(m_index + count == m_cursor);
		}
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

	for (auto& label : m_vLabel)
	{
		label->Render(pView);
	}

	return 0;
}

bool SystemChoiceDialog::IsActive() const
{
	return m_isActive;
}

