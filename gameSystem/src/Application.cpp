#include "Equisetum2.h"
#include "Application.hpp"

#include "Object.hpp"

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>


int Application::Main()
{
	bool isError = false;

	Logger::SetPriority(LogLevel::Debug);
	Logger::SetCallback([&isError](LogLevel level, const char* str)->bool {
		if (!isError && level >= LogLevel::Error)
		{
			// ���o�C�G���[�̓A���[�g���o���ď������~����B
			// �A���[�g��1�񂾂��\������悤�ɂ���B
			// �ق��̂̓��O������΂킩��悤�ɂ��Ă����B
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

	OnInit();

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

		if (!isError)
		{
			OnUpdate();
			OnDraw(m_renderer);
		}

		m_renderer->Present();

		Node::GC();

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

	Object::DestroyThemAll();

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

void Application::SetTargetFps(int32_t fps)
{
	m_fpsMaker->SetTargetFps(fps);
}

int32_t Application::NowFps(void) const
{
	return m_fpsCounter->Fps();
}