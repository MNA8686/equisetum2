#include "SystemChoiceDialog.hpp"
#include "Application.hpp"

std::shared_ptr<SystemChoiceDialog> SystemChoiceDialog::Create(const String & title, const std::vector<String>& vItem, const std::function<void(int index, const String&item)> cb)
{
	int layer = 1;

	EQ_DURING
	{
		if (!cb)
		{
			EQ_THROW(u8"�R�[���o�b�N���ݒ肳��Ă��܂���B");
		}

		auto inst = std::make_shared<SystemChoiceDialog>();
		if (!inst)
		{
			EQ_THROW(u8"�C���X�^���X�̍쐬�Ɏ��s���܂����B");
		}

		inst->m_cb = cb;
		inst->m_title = title;
		inst->m_vItem = vItem;
		inst->m_isActive = true;

		inst->m_label = SystemWidgetLabel::Create(title);
		if (!inst->m_label)
		{
			EQ_THROW(u8"���x���̍쐬�Ɏ��s���܂����B");
		}
		inst->m_label->SetPreset(u8" " + title);

		if (!inst->m_label->SetText(title))
		{
			EQ_THROW(u8"���x���̍쐬�Ɏ��s���܂����B");
		}
		inst->m_label->Setlayer(layer);

		// �O�g
		inst->m_rectRendererBorder = GetApplication()->GetRenderer()->CreateRenderObject<RectRenderer>();
		inst->m_rectRendererBorder->SetBlendMode(BlendMode::Blend);
		inst->m_rectRendererBorder->SetColor(Color{ 0, 180, 0, 200 });
		inst->m_rectRendererBorder->SetLayer(layer);

		// ���g
		inst->m_rectRendererInnerFrame = GetApplication()->GetRenderer()->CreateRenderObject<RectRenderer>();
		inst->m_rectRendererInnerFrame->SetBlendMode(BlendMode::Blend);
		inst->m_rectRendererInnerFrame->SetColor(Color{ 32, 200, 32, 200 });
		inst->m_rectRendererInnerFrame->SetLayer(layer);

		// �O�g�Ɠ��g�̃T�C�Y���Z�o
		inst->CalculateBorder();

		// �^�C�g���ݒ�
		inst->m_label->SetPivot({ 0.5f, 0.5f });
		inst->m_label->SetPos({ inst->m_rectBorder.x + inst->m_rectBorder.width / 2,
								inst->m_rectBorder.y + inst->textMargin / 2.f });

		// �A�C�e���\�����쐬
		{
			std::shared_ptr<BitmapFont> bitmapFont = inst->MakeBitmapFont();
			if (!bitmapFont)
			{
				EQ_THROW(u8"�r�b�g�}�b�v�t�H���g�̍쐬�Ɏ��s���܂����B");
			}

			// �ő�s���Z�o
			inst->m_maxRow = static_cast<int32_t>(inst->m_rectInnerFrame.height / inst->textSize);
			
			// �s�����������x�����쐬����
			for (int32_t count = 0; count < inst->m_maxRow; count++)
			{
				auto label = SystemWidgetLabelWithFont::CreateWithBitmapFont(u8" ", bitmapFont);
				if (!label)
				{
					EQ_THROW(u8"���x���̍쐬�Ɏ��s���܂����B");
				}
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

std::shared_ptr<BitmapFont> SystemChoiceDialog::MakeBitmapFont()
{
	EQ_DURING
	{
		Size size = Window::Size();
		bool makeBitmap = false;

		if (m_font)
		{
			int32_t oldSize = m_font->GetFontSize();
			
			m_font = GetApplication()->GetSystemFont();
			if (m_font)
			{
				if (oldSize != m_font->GetFontSize())
				{
					makeBitmap = true;
				}
			}
		}
		else
		{
			m_font = GetApplication()->GetSystemFont();
			makeBitmap = true;
		}
		
		if (!m_font)
		{
			EQ_THROW(u8"�V�X�e���t�H���g�̎擾�Ɏ��s���܂����B");
		}

		std::shared_ptr<BitmapFont> bitmapFont;
		
		if (makeBitmap)
		{
			// ���ʂ̃t�H���g���쐬����
			String commonStr = u8" 0123456789[]:";
			for (auto& str : m_vItem)
			{
				commonStr += str;
			}

			bitmapFont = m_font->MakeBitmapFont(commonStr, Color{ 0xff, 0xff, 0xff, 0xff });
			if (!bitmapFont)
			{
				EQ_THROW(u8"�r�b�g�}�b�v�t�H���g�̍쐬�Ɏ��s���܂����B");
			}
		}

		return bitmapFont;
	}
	EQ_HANDLER
	{
		Logger::OutputError(EQ_GET_HANDLER().what());
	}
	EQ_END_HANDLER

	return nullptr;
}

void SystemChoiceDialog::CalculateBorder()
{
	Size windowSize = Window::Size();

	// �O�g
	{
		// �s�N�Z���T�C�Y�����߂�
		Rect rectBorderPixel{
			static_cast<int32_t>(windowSize.x * m_rectBorder.x),
			static_cast<int32_t>(windowSize.y * m_rectBorder.y),
			static_cast<int32_t>(windowSize.x * m_rectBorder.width),
			static_cast<int32_t>(windowSize.y * m_rectBorder.height)
		};

		m_rectRendererBorder->SetRect(rectBorderPixel, true);
	}

	// ���g
	{
		// �s�N�Z���T�C�Y�����߂�
		Rect rectInnerFramePixel{
			static_cast<int32_t>(windowSize.x * m_rectInnerFrame.x),
			static_cast<int32_t>(windowSize.y * m_rectInnerFrame.y),
			static_cast<int32_t>(windowSize.x * m_rectInnerFrame.width),
			static_cast<int32_t>(windowSize.y * m_rectInnerFrame.height)
		};

		m_rectRendererInnerFrame->SetRect(rectInnerFramePixel, true);
	}
}

int SystemChoiceDialog::Do(SystemView * pView)
{
	if(!m_isActive)
	{
		return -1;
	}

	auto size = static_cast<int32_t>(m_vItem.size());

	// ��L�[�����H
	if (KB::KeyUp.IsPress())
	{
		if (KB::KeyUp.IsDown() || KB::KeyUp.PressedDuration() > 300)
		{
			if (m_cursor > 0)
			{
				m_cursor--;
			}

			// �\���J�n�ʒu�����炷
			if (m_cursor < m_index)
			{
				m_index--;
			}
		}
	}
	// ���L�[�����H
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

				// �\���J�n�ʒu�����炷
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

	// �A�C�e����\������
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

	// �O�g�Ɠ��g�̃T�C�Y���Z�o
	CalculateBorder();
	// �O�g�Ɠ��g�������_�[�L���[�ɒǉ�
	GetApplication()->GetRenderer()->AddRenderQueue(m_rectRendererBorder.get());
	GetApplication()->GetRenderer()->AddRenderQueue(m_rectRendererInnerFrame.get());

	// �r�b�g�}�b�v�t�H���g���X�V����Ă����烉�x���ɔ��f������
	auto bitmapFont = MakeBitmapFont();
	if(bitmapFont)
	{
		// �s�����������x�����쐬����
		for (auto& label : m_vLabel)
		{
			label->SetBitmapFont(bitmapFont);
		}
	}

	// ���x���������_�����O����
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

