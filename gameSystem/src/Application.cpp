#include "Equisetum2.h"
#include "Application.hpp"

#include "Object.hpp"
#include "Node.hpp"

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>


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
			static float tran_x = 0;

			if (!next)
			{
				std::shared_ptr<SystemView> nextView;
				for (auto& view : m_vView)
				{
					if (view->GetFocus())
					{
						view->DoView();	// for view test
						nextView = view->GetNextView();
						if (nextView)
						{
							view->SetFocus(false);
						}
						break;
					}
				}

				if (nextView)
				{
					nextView->SetFocus(true);
					nextView->SetPos(PointF{ 1.f, 0 });
					m_vView.push_back(nextView);
					nextView = nullptr;

					next = true;
				}
			}
			else
			{
				tran_x += 0.06f;
				if (tran_x > 1.f)
				{
					tran_x = 1.f;
					next = false;
				}

				int index = 0;
				for (auto& view : m_vView)
				{
					view->SetPos(PointF{index - tran_x, 0});
					index++;
				}

			}

			OnUpdate();

			m_renderer->SetRenderTarget(nullptr);
			m_renderer->Clear({ 128, 128, 0, 0 });
			//OnDraw();
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

