#if !defined(_EQFPSCOUNTER_H_)
#define _EQFPSCOUNTER_H_

#include "TickCounter.h"
#include <memory>

namespace Equisetum2
{
	class FpsCounter
	{
	public:

		// �t�@�N�g���[
		// �����ŃJ�E���^�[�̃��Z�b�g�Ȃǂ��s�����߁A�����ɂ͂��̃N���X��p�̃J�E���^�[��ݒ肷�邱��
		static std::shared_ptr<FpsCounter> Create(std::shared_ptr<TickCounter> tickCounter);

		virtual void Reset();
		virtual void Increase();
		virtual uint32_t Fps() const;

	protected:

		explicit FpsCounter();
		virtual ~FpsCounter();
		virtual void SetTickCounter(std::shared_ptr<TickCounter> tickCounter);

	private:

		uint32_t m_NowFps = 0;		// �J�E���g����FPS
		uint32_t m_RecentFps = 0;		// 1�b�O��FPS
		std::shared_ptr<TickCounter> m_TickCounter;

		FpsCounter(const FpsCounter&);				// �R�s�[�R���X�g���N�^����
		FpsCounter& operator= (const FpsCounter&);	// �R�s�[�R���X�g���N�^����
	};
}

#endif
