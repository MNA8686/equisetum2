#include "SystemWidgetMenu.hpp"
#include "Application.hpp"

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

Rect SystemWidgetMenu::GetBox() const
{
	return{};
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

