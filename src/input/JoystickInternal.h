#if !defined(_EQJOYSTICKINTERNAL_H_)
#define _EQJOYSTICKINTERNAL_H_

#include "system/Singleton.h"
#include "input/IKeyState.h"
#include "input/Joystick.h"
#include <memory>

namespace Equisetum2
{
	// �W���C�X�e�B�b�N�̃|�[�e�B���O�N���X
	// ���݂��Ȃ��W���C�X�e�B�b�N�̔ԍ�����͂��Ă��N���b�V�����Ȃ����Ƃ��ۏ؂���Ă���

	class JoystickInternal final
	{
	public:
		friend Singleton<JoystickInternal>;	// �V���O���g������C���X�^���X���쐬���Ă��炦��悤�ɂ���

		void Prepare();
		void Update();

		bool IsConnected(int joyIndex) const;
		int16_t Axis(int joyIndex, int num) const;
		HatState Hat(int joyIndex, int num) const;
		const stKeyState& Button(int joyIndex, int num) const;

		int NumAxes(int joyIndex) const;
		int NumButtons(int joyIndex) const;
		int NumHats(int joyIndex) const;
		const std::string Name(int joyIndex) const;
		int NumJoysticks() const;

	private:

		class Impl;
		std::shared_ptr<Impl> m_Impl;
		JoystickInternal();	// �C���X�^���X�쐬����
	};
}

#endif
