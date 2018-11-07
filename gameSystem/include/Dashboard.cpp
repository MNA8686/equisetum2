#include "Dashboard.hpp"
#include "SystemWidgetLabel.hpp"

std::shared_ptr<Dashboard> Dashboard::CreateWithView(std::shared_ptr<SystemView> pView)
{
	EQ_DURING
	{
		if (!pView)
		{
			EQ_THROW(u8"ビューが設定されていません。");
		}

		auto inst = std::make_shared<Dashboard>();
		if (!inst)
		{
			EQ_THROW(u8"インスタンスの作成に失敗しました。");
		}

		// ビューを設定する
		pView->SetFocus(true);
		inst->m_vView.push_back(pView);

		// パンくずを作成する
		inst->m_labelBreadcrumb = SystemWidgetLabel::Create(u8"Breadcrumb");
		if (!inst->m_labelBreadcrumb)
		{
			EQ_THROW(u8"パンくずの作成に失敗しました。");
		}
		inst->m_labelBreadcrumb->SetPivot({ 0.f, 0.5f });
		inst->m_labelBreadcrumb->SetPos({ 0.05f, 0.08f });

		inst->RebuildBreadcrumb();

		return inst;
	}
	EQ_HANDLER
	{
		Logger::OutputError(EQ_GET_HANDLER().what());
	}
	EQ_END_HANDLER

	return nullptr;
}

void Dashboard::SetPos(const PointF & pos)
{
	m_pos = pos;
}

PointF Dashboard::GetPos() const
{
	return m_pos;
}

int Dashboard::Do()
{
	const int frameMax = 16;	// 何フレームかけてスクロールするか？
	int ret = 0;

	// ビュー移動中？
	if (IsTransition())
	{
		if (m_next)
		{
			// ビューを移動
			m_nowPos.x -= 1.f / frameMax;
			// 移動完了？
			if (m_nowPos.x < m_destPos.x)
			{
				m_nowPos.x = m_destPos.x;
				RebuildBreadcrumb();

				m_next = false;
			}

		}
		else if (m_prev)
		{
			// ビューを移動
			m_nowPos.x += 1.f / frameMax;
			// 移動完了？
			if (m_nowPos.x > m_destPos.x)
			{
				m_nowPos.x = m_destPos.x;
				m_vView.pop_back();
				m_vView[m_vView.size() - 1]->SetFocus(true);
				RebuildBreadcrumb();

				m_prev = false;
			}
		}

		// ビューの座標を更新
		int index = 0;
		for (auto& view : m_vView)
		{
			view->SetPos(PointF{ m_nowPos.x + index * 1.f, 0 });
			index++;
		}
		
		return ret;
	}

	std::shared_ptr<SystemView> nextView;
	// ビューの処理を行う
	for (auto& view : m_vView)
	{
		if (view->GetFocus())
		{
			view->DoView();
			if (view->GetStat() == SystemView::Stat::Push)
			{
				nextView = view->GetNextView();
				if (nextView)
				{
					view->SetFocus(false);
					m_next = true;
				}
			}
			else if (view->GetStat() == SystemView::Stat::Pop)
			{
				m_prev = true;
			}
			break;
		}
	}

	// ビューをスクロールさせることになった？
	if (IsTransition())
	{
		if (m_next)
		{
			// 新しいビューに対して設定を行う
			nextView->SetFocus(true);
			nextView->SetPos(PointF{ 1.f, 0 });
			m_vView.push_back(nextView);

			// 1画面分ずらす
			m_destPos.x -= 1.f;
		}
		else if (m_prev)
		{
			if (m_vView.size() <= 1)
			{
				m_prev = false;
				ret = 1;
			}
			else
			{
				// 1画面分ずらす
				m_destPos.x += 1.f;
			}
		}
	}

	return ret;
}

int Dashboard::Render()
{
	// パンくず表示
	m_labelBreadcrumb->Render(nullptr);

	for (auto& view : m_vView)
	{
		auto pos = view->GetPos();
		// 画面内に存在する？
		if (pos.x > -1.f && pos.x < 1.f)
		{
			view->RenderView();
		}
	}

	return 0;
}

void Dashboard::RebuildBreadcrumb()
{
	String path;

	// パンくず文字列生成
	int count = 0;
	for (auto& view : m_vView)
	{
		if (count > 0)
		{
			path += " > ";
		}

		// ビューの名前を取得し、連結する
		path += view->GetName();

		count++;
	}

	// パンくずの文字列設定
	m_labelBreadcrumb->SetPreset(path);
	m_labelBreadcrumb->SetText(path);
}

bool Dashboard::IsTransition() const
{
	return m_next | m_prev;
}


