#include "Equisetum2.h"
#include "Application.hpp"

#include "Object.hpp"
#include "Node.hpp"

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>

#include "SystemWidgetLabel.hpp"
#include "Dashboard.hpp"
#include "SystemViewTopMenu.hpp"
#include <iomanip>

static const String logfile = "log.txt";

int Application::Main()
{
	bool isError = false;
	std::shared_ptr<IStream> logStream;

	Logger::SetPriority(LogLevel::Debug);

	Logger::SetCallback([&isError, &logStream, this](LogLevel level, const char* str)->bool {
		if (!isError)
		{
			if (level >= LogLevel::Error)
			{
				// ���o�C�G���[�̓A���[�g���o���ď������~����B
				// �A���[�g��1�񂾂��\������悤�ɂ���B
				// �ق��̂̓��O������΂킩��悤�ɂ��Ă����B
				Window::ShowAlert(str);
				isError = true;
			}
		}

		if (level >= LogLevel::Info)
		{
			// ���O���t�@�C���ɏo�͂���
			if (logStream)
			{
				static const char* tbl[] =
				{
					"?",
					"V",
					"D",
					"I",
					"W",
					"E",
					"C",
				};

				#pragma warning(push)
				#pragma warning(disable: 4996)
				std::time_t t = std::time(nullptr);
				const std::tm* lt = std::localtime(&t);
				#pragma warning(pop)

				// ���O���x���Ǝ�����擪�ɒǉ�����
				String errorStr = String::Sprintf("%s %d/%02d/%02d %02d:%02d:%02d %s", tbl[static_cast<int>(level)], 1900 + lt->tm_year, lt->tm_mon + 1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec, str);
				// ���s�������Ė�����Ή��s��ǉ�����
				if (*(errorStr.rbegin()) != '\n')
				{
					errorStr += "\n";
				}

				logStream->Write(reinterpret_cast<const uint8_t*>(errorStr.c_str()), errorStr.size());
			}
		}
		return false;
	});

	// ��̃��O�t�@�C�����쐬����
	Optional<String> path = Path::GetPrivateDocumentPath(GetOrganizationName(), GetApplicationName());
	if (path)
	{
		logStream = FileStream::NewFileFromPath(*path + logfile);
	}

	OnCreate();

	m_sysTimer = SystemTimer::Create();
	m_fpsMakerTick = TickCounter::Create(m_sysTimer);
	m_fpsMaker = FpsPaceMaker::Create(m_fpsMakerTick, m_fps);

	m_fpsCounterTick = TickCounter::Create(m_sysTimer);
	m_fpsCounter = FpsCounter::Create(m_fpsCounterTick);

	m_renderer = Renderer::Create();

	// �_�b�V���{�[�h
	auto view = TopMenu::Create(u8"TOP");
	if (!view)
	{
		return -1;
	}
	m_dashboard = Dashboard::CreateWithView(view);

	// FPS
	auto labelFps = SystemWidgetLabel::Create(u8" 0123456789/");
	labelFps->SetPivot({ 1.0f, 0.5f });
	labelFps->SetPos({ 0.98f, 0.95f });

	bool isModeChange = true;
	bool atDashboard = true;
	bool pause = false;

	while (!m_isQuit)
	{
		// �V�X�e������
		if (!System::Update())
		{
			break;
		}

		// �t���X�N���[���؂�ւ�
		static bool full = true;
		if (KB::KeyF.IsDown())
		{
			Window::SetFullscreen(full);
			full = !full;
		}

#if 0
		// �|�[�Y�؂�ւ�
		if (KB::KeyP.IsDown())
		{
			pause = !pause;
		}

		// �|�[�Y�@�\
		if (pause)
		{
			System::Sleep(1);
			continue;
		}
#endif

		if (!isError)
		{
			if (isModeChange)
			{
				if (atDashboard)
				{
					OnQuit();
					Node<Object>::DestroyThemAll();
					Singleton<NodePool<Object>>::GetInstance()->Reset();
				}
				else
				{
					OnInit();
				}

				isModeChange = false;
			}

			if (atDashboard)
			{
				int ret = m_dashboard->Do();
				if (ret > 0)
				{
					atDashboard = false;
					isModeChange = true;
				}

				m_renderer->SetRenderTarget(nullptr);
				m_renderer->Clear({ 5, 32, 18, 0 });
				m_dashboard->Render();
			}
			else
			{
				OnUpdate();
				if (KB::KeyT.IsDown())
				{
					atDashboard = true;
					isModeChange = true;
				}

				OnDraw();
			}

			// FPS�\��
			labelFps->SetText(String::Sprintf("%d / %d", m_fpsCounter->Fps(), m_fpsMaker->TargetFps()));
			labelFps->Setlayer(Renderer::LayerMax - 1);
			labelFps->Render(nullptr);

			// �����_�[�L���[�̓��e����������
			m_renderer->Render();
		}

		m_renderer->Present();

		Node<Object>::GC();

		// �K�莞�ԑ҂�
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

		// FPS���J�E���g�A�b�v
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
		String arg = String::Sprintf(u8"system/mgenplus-1pp-heavy?%d", fontSize);
		
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
