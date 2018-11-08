#include "SystemWidgetChoice.hpp"
#include "SystemChoiceDialog.hpp"
#include "Application.hpp"

std::shared_ptr<SystemWidgetChoice> SystemWidgetChoice::Create(const String& label, const std::function<std::vector<String>()> cb)
{
	EQ_DURING
	{
		auto inst = std::shared_ptr<SystemWidgetChoice>(new SystemWidgetChoice);
		if (!inst)
		{
			EQ_THROW(u8"インスタンスの作成に失敗しました。");
		}

		inst->m_text = label;
		inst->m_cb = cb;
		inst->m_label = SystemWidgetLabel::Create(label);
		if (!inst->m_label)
		{
			EQ_THROW(u8"ラベルの作成に失敗しました。");
		}
		inst->m_label->SetPreset(u8" " + label);

		if (!inst->m_label->SetText(label))
		{
			EQ_THROW(u8"ラベルの作成に失敗しました。");
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

int SystemWidgetChoice::Do(SystemView* pView)
{
	m_label->SetPos(m_pos);

	if (m_exclusive)
	{
		if (m_dialog->IsActive())
		{
			m_dialog->Do(nullptr);
		}
		else
		{
			m_exclusive = false;
		}
	}
	else
	{
		// 上キー押下？
		if (KB::KeyUp.IsDown())
		{
			Prev();
		}
		// 下キー押下？
		else if (KB::KeyDown.IsDown())
		{
			Next();
		}
		// Enterキー押下？
		else if (KB::KeyEnter.IsDown())
		{
			m_exclusive = true;
			// アイテム一覧を取得する
			m_vItem = m_cb();
			// ダイアログを作成する
			m_dialog = SystemChoiceDialog::Create(m_text, m_vItem, [this](int index, const String& item) {
				if (index >= 0)
				{
					m_chooseIndex = index;
					m_label->SetPreset(u8" []" + m_text + item);
					m_label->SetText(m_text + u8" [" + item + u8"]");
				}
			});
		}
	}

	return 0;
}

int SystemWidgetChoice::Render(const SystemView* pView)
{
	// label
	Size size = Window::Size();

	m_label->Render(pView);

	if (m_exclusive &&
		m_dialog)
	{
		m_dialog->Render(nullptr);
	}

	return 0;
}

Rect SystemWidgetChoice::GetBox() const
{
	return m_label->GetBox();
}

void SystemWidgetChoice::SetPos(const PointF & pos)
{
	SystemWidget::SetPos(pos);
	m_label->SetPos(pos);
}

int32_t SystemWidgetChoice::GetChooseIndex() const
{
	return m_chooseIndex;
}

