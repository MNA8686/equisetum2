/**
* @file Audio.hpp
* @brief �I�[�f�B�I�����N���X
* @author MNA
* @date 2017/09/07 �V�K�쐬
*/
#if !defined(_EQAUDIO_H_)
#define _EQAUDIO_H_

namespace Equisetum2
{
	class Audio
	{
	public:
		static const double MaxVolume;	/// ���̃��C�u�����Ŏg�p���鉹�̃{�����[���ő�l
		static const double MinVolume;	/// ���̃��C�u�����Ŏg�p���鉹�̃{�����[���ŏ��l

		/**
		* @brief �S�ĉ����ꎞ��~����
		* @detail �����ŎQ�ƃJ�E���^���Ǘ����Ă��邽�߁A������Ă�ł����Ȃ�
		*/
		static void Pause();

		/**
		* @brief �S�Ẳ��̍Đ����ĊJ����
		* @detail �����ŎQ�ƃJ�E���^���Ǘ����Ă��邽�߁A������Ă�ł����Ȃ�
		*/
		static void Resume();

		/**
		* @brief �S�Ẳ����ꎞ��~�����ǂ������擾����
		* @return �ꎞ�����ǂ���
		*/
		static bool IsPaused();

		/**
		* @brief �}�X�^�[�{�����[����ݒ肷��
		* @param volume �{�����[���ݒ� 0�`1.0
		* @return ����
		*/
		static bool SetVolume(double volume);

		/**
		* @brief �}�X�^�[�{�����[�����擾����
		* @return �{�����[���ݒ� 0�`1.0
		*/
		static double Volume();
	};
}

#endif
