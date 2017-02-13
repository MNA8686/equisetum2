#if !defined(_EQFPSPACEMAKER_H_)
#define _EQFPSPACEMAKER_H_

#include "TickCounter.h"
#include <memory>

namespace Equisetum2
{
	class FpsPaceMaker
	{
	public:

		// �t�@�N�g���[
		// �����ŃJ�E���^�[�̃��Z�b�g�Ȃǂ��s�����߁A�����ɂ͂��̃N���X��p�̃J�E���^�[��ݒ肷�邱��
		static std::shared_ptr<FpsPaceMaker> Create(std::shared_ptr<TickCounter> tickCounter, int32_t fps);

		virtual void Reset();
		virtual void SetTargetFps(uint32_t fps);
		virtual uint32_t TargetFps() const;
		virtual uint32_t RemainTicks();	// ���̃t���[���J�n�܂ł̎c�莞�Ԃ��擾����(ms)
		virtual uint32_t ElapsedFrames();	// ���t���[���o�߂�����(RemainTicks�̂��ƂŌĂԂ���)

	protected:

		explicit FpsPaceMaker();
		virtual ~FpsPaceMaker();
		virtual void SetTickCounter(std::shared_ptr<TickCounter> tickCounter);

	private:

		uint32_t m_TargetFps = 0;		// ���ݐݒ肳��Ă���FPS
		uint32_t m_fixdWaitTickPerFrame = 0;	// 1�t���[��������̑҂�����(ms)
		uint32_t m_fixdCarryTick = 0;	// �J��z���ꂽ����
		uint32_t m_ElapsedFrame = 0;	// �o�߃t���[����
		std::shared_ptr<TickCounter> m_TickCounter;

		FpsPaceMaker(const FpsPaceMaker&);				// �R�s�[�R���X�g���N�^����
		FpsPaceMaker& operator= (const FpsPaceMaker&);	// �R�s�[�R���X�g���N�^����

		const int32_t m_FractionalPart = 16;	// �Œ菬���_�̏��������̌���
	};
}

#endif
