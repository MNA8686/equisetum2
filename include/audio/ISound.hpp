/**
* @file ISound.hpp
* @brief ������̒��ۃN���X
* @author MNA
* @date 2017/09/07 �V�K�쐬
*/
#if !defined(_EQISOUND_H_)
#define _EQISOUND_H_

namespace Equisetum2
{
	/**
	* ������̒��ۃN���X�B<br>
	* ���������N���X�͂����h�����Ďg���B
	*/
	class ISound
	{
	public:
		/**
		* @brief �����Đ�����
		* @param loop ���[�v���������Ƃ��ɐݒ肷��
		* @return ����
		*/
		virtual bool Play(bool loop) = 0;

		/**
		* @brief �����~����
		* @return ����
		*/
		virtual bool Stop() = 0;

		/**
		* @brief �����ꎞ��~����
		* @detail �����ŎQ�ƃJ�E���^���Ǘ����Ă��邽�߁A������Ă�ł����Ȃ�
		*/
		virtual void Pause() = 0;

		/**
		* @brief ���̍Đ����ĊJ����
		* @detail �����ŎQ�ƃJ�E���^���Ǘ����Ă��邽�߁A������Ă�ł����Ȃ�
		*/
		virtual void Resume() = 0;

		/**
		* @brief �����Đ������ǂ������擾����
		* @return �Đ������ǂ���
		*/
		virtual bool IsPlaying() const = 0;

		/**
		* @brief �����ꎞ��~�����ǂ������擾����
		* @return �ꎞ��~�����ǂ���
		*/
		virtual bool IsPaused() const = 0;

		/**
		* @brief ���̃{�����[����ݒ肷��
		* @param volume �{�����[���ݒ� 0�`1.0
		* @return ����
		*/
		virtual bool SetVolume(double volume) = 0;

		/**
		* @brief ���̃{�����[�����擾����
		* @return �{�����[���ݒ� 0�`1.0
		*/
		virtual double Volume() const = 0;
	};
}

#endif
