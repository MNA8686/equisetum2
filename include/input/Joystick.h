#if !defined(_EQJOYSTICK_H_)
#define _EQJOYSTICK_H_

#include "input/IKeyState.h"
#include "input/Key.h"
#include <memory>
#include <functional>

namespace Equisetum2
{
	// �W���C�X�e�B�b�N�̃A�N�Z�X�N���X
	// ���݂��Ȃ��ԍ��ɃA�N�Z�X���Ă��N���b�V�����Ȃ����Ƃ��ۏ؂���Ă���
	// �}���������Ă��C���X�^���X����蒼���K�v�͂Ȃ�

	typedef struct
	{
		std::function<void(int index)> OnConnected;
		std::function<void(int index)> OnRemoved;
	}stJoystickEvent;

	class Joystick
	{
	public:

		static std::shared_ptr<Joystick> CreateByIndex(int index);

		bool IsConnected() const;
		int16_t Axis(int num) const;
		const Key HatUp(int num = 0) const;
		const Key HatDown(int num = 0) const;
		const Key HatLeft(int num = 0) const;
		const Key HatRight(int num = 0) const;
		const Key Button(int num) const;

		int NumAxes() const;
		int NumButtons() const;
		int NumHats() const;
		const std::string Name() const;
		int Index();
		void SetEventListener(const stJoystickEvent& listener);

		static int NumJoysticks();
		
		~Joystick();

	protected:

		Joystick(int index);	// �C���X�^���X�쐬����

	private:

		int m_Index = 0;
	};
}

#endif
