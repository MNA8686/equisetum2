#if !defined(_EQTIMER_H_)
#define _EQTIMER_H_

#include <cstdint>

namespace Equisetum2
{
	// ���Ԃ����ނ����̃V���v���ȃ^�C�}
	// Update()���s�����ƂŁA�����ŕێ����Ă��鎞�����X�V�����
	// ����Update()���s���܂œǂݏo���鎞���̒l�͕ς��Ȃ����Ƃ��ۏ؂���Ă���
	class ITimer
	{
	public:
		virtual bool Update() = 0;	// 1�t���[���Ɉ�x�^�C�}�̒l���X�V����
		virtual uint32_t Ticks() const = 0;		// property: �ێ�����Ă��鎞��(ms)
	};
}

#endif
