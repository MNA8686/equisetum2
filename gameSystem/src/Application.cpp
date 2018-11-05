#include "Equisetum2.h"
#include "Application.hpp"

#include "Object.hpp"
#include "Node.hpp"

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>

#include "SystemWidgetLabel.hpp"

int Application::Main()
{
	bool isError = false;

	Logger::SetPriority(LogLevel::Debug);
	Logger::SetCallback([&isError](LogLevel level, const char* str)->bool {
		if (!isError && level >= LogLevel::Error)
		{
			// ヤバイエラーはアラートを出して処理を停止する。
			// アラートは1回だけ表示するようにする。
			// ほかのはログを見ればわかるようにしておく。
			Window::ShowAlert(str);
			isError = true;
		}
		return false;
	});

	m_sysTimer = SystemTimer::Create();
	m_fpsMakerTick = TickCounter::Create(m_sysTimer);
	m_fpsMaker = FpsPaceMaker::Create(m_fpsMakerTick, m_fps);

	m_fpsCounterTick = TickCounter::Create(m_sysTimer);
	m_fpsCounter = FpsCounter::Create(m_fpsCounterTick);

	bool pause = false;

	m_renderer = Renderer::Create();

	// viewテスト
	{
		auto view = AssetMenu::Create(u8"アセットテスト");
		view->SetFocus(true);
		m_vView.push_back(view);

		//m_view = AssetMenu::Create(u8"アセットテスト");
	}
	auto labelFps = SystemWidgetLabel::Create(u8" 0123456789/");
	labelFps->SetPivot({ 1.0f, 0.5f });
	labelFps->SetPos({ 0.98f, 0.95f });
	
	auto labelBreadcrumb = SystemWidgetLabel::Create(u8"Breadcrumb");
	labelBreadcrumb->SetPivot({ 0.f, 0.5f });
	labelBreadcrumb->SetPos({ 0.05f, 0.08f });
	
	OnInit();

	while (!m_isQuit)
	{
		// システム処理
		if (!System::Update())
		{
			break;
		}

		// フルスクリーン切り替え
		static bool full = true;
		if (KB::KeyF.IsDown())
		{
			Window::SetFullscreen(full);
			full = !full;
		}

#if 0
		// ポーズ切り替え
		if (KB::KeyP.IsDown())
		{
			pause = !pause;
		}

		// ポーズ機能
		if (pause)
		{
			System::Sleep(1);
			continue;
		}
#endif

		if (!isError)
		{
			static bool next = false;
			static bool prev = false;
			//static float tran_x = 0;
			static float now_x = 0;
			static float dest_x = 0;

			if (next)
			{
				now_x -= 0.06f;
				if (now_x < dest_x)
				{
					now_x = dest_x;
					next = false;
				}

				int index = 0;
				for (auto& view : m_vView)
				{
					view->SetPos(PointF{ index * 1.f + now_x, 0 });
					index++;
				}
			}
			else if (prev)
			{
				now_x += 0.06f;
				if (now_x > dest_x)
				{
					now_x = dest_x;
					m_vView.pop_back();
					m_vView[m_vView.size() - 1]->SetFocus(true);
					prev = false;
				}

				int index = 0;
				for (auto& view : m_vView)
				{
					view->SetPos(PointF{ index * 1.f + now_x, 0 });
					index++;
				}
			}
			else
			{
				std::shared_ptr<SystemView> nextView;
				for (auto& view : m_vView)
				{
					if (view->GetFocus())
					{
						view->DoView();	// for view test
						if (view->GetStat() == SystemView::Stat::Push)
						{
							nextView = view->GetNextView();
							if (nextView)
							{
								view->SetFocus(false);
								next = true;
							}
						}
						else if (view->GetStat() == SystemView::Stat::Pop)
						{
							prev = true;
						}
						break;
					}
				}

				if (next)
				{
					nextView->SetFocus(true);
					nextView->SetPos(PointF{ 1.f, 0 });
					m_vView.push_back(nextView);
					nextView = nullptr;
					dest_x -= 1.f;
				}
				else if (prev)
				{
					if (m_vView.size() > 0)
					{
						prev = true;
						dest_x += 1.f;
					}
				}

				{
					String path;

					int count = 0;
					for (auto& view : m_vView)
					{
						if (count > 0)
						{
							path += " > ";
						}

						path += view->GetName();

						count++;
					}

					labelBreadcrumb->SetPreset(path);
					labelBreadcrumb->SetText(path);
				}
			}

			OnUpdate();

			m_renderer->SetRenderTarget(nullptr);
			m_renderer->Clear({ 128, 128, 0, 0 });
			//OnDraw();
			labelFps->SetText(String::Sprintf("%d / %d", m_fpsCounter->Fps(), m_fpsMaker->TargetFps()));
			labelFps->Render(nullptr);
			labelBreadcrumb->Render(nullptr);
			
			for (auto& view : m_vView)
			{
				auto pos = view->GetPos();
				// 画面内に存在する？
				if (pos.x > -1.f && pos.x < 1.f)
				{
					view->RenderView();	// for view test
				}
			}
			m_renderer->Render();
		}

		m_renderer->Present();

		Node<Object>::GC();

		// 規定時間待ち
		while (true)
		{
			m_sysTimer->Update();

			uint32_t remain = m_fpsMaker->RemainTicks();

			if (remain >= 2)
			{
				System::Sleep(1);
			}
			else if (remain == 0)
			{
				break;
			}
		}

		// FPSをカウントアップ
		m_fpsCounter->Increase();
	}

	OnQuit();

	Node<Object>::DestroyThemAll();

	return 0;
}

bool Application::OnActive(bool active)
{
	return true;
}

void Application::Quit()
{
	m_isQuit = true;
}

std::shared_ptr<Renderer>& Application::GetRenderer(void)
{
	return m_renderer;
}

std::shared_ptr<FontManager>& Application::GetSystemFont(void)
{
	if (!m_fontManager)
	{
		int fontSize = Window::Size().y / 30;
		String arg = String::Sprintf(u8"mgenplus-1pp-heavy?%d", fontSize);
		
		m_fontManager = Singleton<AssetManager>::GetInstance()->Load<FontManager>(arg);
	}

	return m_fontManager;
}

void Application::SetTargetFps(int32_t fps)
{
	m_fpsMaker->SetTargetFps(fps);
}

int32_t Application::NowFps(void) const
{
	return m_fpsCounter->Fps();
}

