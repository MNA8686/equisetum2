#include "Dashboard.hpp"
#include "SystemWidgetLabel.hpp"

std::shared_ptr<Dashboard> Dashboard::CreateWithView(std::shared_ptr<SystemView> pView)
{
	EQ_DURING
	{
		if (!pView)
		{
			EQ_THROW(u8"�r���[���ݒ肳��Ă��܂���B");
		}

		auto inst = std::make_shared<Dashboard>();
		if (!inst)
		{
			EQ_THROW(u8"�C���X�^���X�̍쐬�Ɏ��s���܂����B");
		}

		// �r���[��ݒ肷��
		pView->SetFocus(true);
		inst->m_vView.push_back(pView);

		// �p���������쐬����
		inst->m_labelBreadcrumb = SystemWidgetLabel::Create(u8"Breadcrumb");
		if (!inst->m_labelBreadcrumb)
		{
			EQ_THROW(u8"�p�������̍쐬�Ɏ��s���܂����B");
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
	const int frameMax = 16;	// ���t���[�������ăX�N���[�����邩�H
	int ret = 0;

	// �r���[�ړ����H
	if (IsTransition())
	{
		if (m_next)
		{
			// �r���[���ړ�
			m_nowPos.x -= 1.f / frameMax;
			// �ړ������H
			if (m_nowPos.x < m_destPos.x)
			{
				m_nowPos.x = m_destPos.x;
				RebuildBreadcrumb();

				m_next = false;
			}

		}
		else if (m_prev)
		{
			// �r���[���ړ�
			m_nowPos.x += 1.f / frameMax;
			// �ړ������H
			if (m_nowPos.x > m_destPos.x)
			{
				m_nowPos.x = m_destPos.x;
				m_vView.pop_back();
				m_vView[m_vView.size() - 1]->SetFocus(true);
				RebuildBreadcrumb();

				m_prev = false;
			}
		}

		// �r���[�̍��W���X�V
		int index = 0;
		for (auto& view : m_vView)
		{
			view->SetPos(PointF{ m_nowPos.x + index * 1.f, 0 });
			index++;
		}
		
		return ret;
	}

	std::shared_ptr<SystemView> nextView;
	// �r���[�̏������s��
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

	// �r���[���X�N���[�������邱�ƂɂȂ����H
	if (IsTransition())
	{
		if (m_next)
		{
			// �V�����r���[�ɑ΂��Đݒ���s��
			nextView->SetFocus(true);
			nextView->SetPos(PointF{ 1.f, 0 });
			m_vView.push_back(nextView);

			// 1��ʕ����炷
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
				// 1��ʕ����炷
				m_destPos.x += 1.f;
			}
		}
	}

	return ret;
}

int Dashboard::Render()
{
	// �p�������\��
	m_labelBreadcrumb->Render(nullptr);

	for (auto& view : m_vView)
	{
		auto pos = view->GetPos();
		// ��ʓ��ɑ��݂���H
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

	// �p�����������񐶐�
	int count = 0;
	for (auto& view : m_vView)
	{
		if (count > 0)
		{
			path += " > ";
		}

		// �r���[�̖��O���擾���A�A������
		path += view->GetName();

		count++;
	}

	// �p�������̕�����ݒ�
	m_labelBreadcrumb->SetPreset(path);
	m_labelBreadcrumb->SetText(path);
}

bool Dashboard::IsTransition() const
{
	return m_next | m_prev;
}


