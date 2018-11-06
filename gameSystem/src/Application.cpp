#include "Equisetum2.h"
#include "Application.hpp"

#include "Object.hpp"
#include "Node.hpp"

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>

#include "SystemWidgetLabel.hpp"
#include "Dashboard.hpp"

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

	bool atDashboard = true;
	bool pause = false;

	m_renderer = Renderer::Create();

	// ダッシュボード
	auto view = AssetMenu::Create(u8"アセットテスト");
	m_dashboard = Dashboard::CreateWithView(view);

	// FPS
	auto labelFps = SystemWidgetLabel::Create(u8" 0123456789/");
	labelFps->SetPivot({ 1.0f, 0.5f });
	labelFps->SetPos({ 0.98f, 0.95f });

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
			if (atDashboard)
			{
				int ret = m_dashboard->Do();
				if (ret > 0)
				{
					atDashboard = false;
				}

				m_renderer->SetRenderTarget(nullptr);
				m_renderer->Clear({ 32, 150, 32, 0 });
				m_dashboard->Render();
			}
			else
			{
				OnUpdate();
				if (KB::KeyT.IsDown())
				{
					atDashboard = true;
				}

				OnDraw();
			}

			// FPS表示
			labelFps->SetText(String::Sprintf("%d / %d", m_fpsCounter->Fps(), m_fpsMaker->TargetFps()));
			labelFps->Render(nullptr);

			// レンダーキューの内容を処理する
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

