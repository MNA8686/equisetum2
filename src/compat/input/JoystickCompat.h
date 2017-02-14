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

	class JoystickCompat final
	{
	public:
		friend Singleton<JoystickCompat>;	// �V���O���g������C���X�^���X���쐬���Ă��炦��悤�ɂ���

		void Prepare();
		void Update();

		bool IsConnected(int joyIndex) const;
		int16_t Axis(int joyIndex, int num) const;
		const stKeyState& HatUp(int joyIndex, int num) const;
		const stKeyState& HatDown(int joyIndex, int num) const;
		const stKeyState& HatLeft(int joyIndex, int num) const;
		const stKeyState& HatRight(int joyIndex, int num) const;
		const stKeyState& Button(int joyIndex, int num) const;

		int NumAxes(int joyIndex) const;
		int NumButtons(int joyIndex) const;
		int NumHats(int joyIndex) const;
		const std::string Name(int joyIndex) const;
		void SetEventListener(int joyIndex, const stJoystickEvent& listener, void* key);
		int NumJoysticks() const;

	private:

		class Impl;
		std::shared_ptr<Impl> m_Impl;
		JoystickCompat();	// �C���X�^���X�쐬����
	};
}

#endif
