/**
* @file SE.hpp
* @brief SE�Đ��N���X
* @author MNA
* @date 2017/09/07 �V�K�쐬
*/
#if !defined(_EQSE_H_)
#define _EQSE_H_

#include "audio/ISound.hpp"
#include "stream/IStream.h"
#include <memory>

namespace Equisetum2
{
	/**
	* SE���Đ����邽�߂̃N���X�B
	*/
	class SE : public ISound
	{
	public:
		/**
		* @brief �X�g���[������SE���쐬���A�Đ�����C���X�^���X���쐬����
		* @param stream ���̓X�g���[��(�ǂݏo���A�V�[�N�������K�v)
		* @return �쐬�����C���X�^���X
		*/
		static std::shared_ptr<SE> CreateFromStream(const std::shared_ptr<IStream> stream);

		bool Play(bool loop) override;
		bool Stop() override;
		void Pause() override;
		void Resume() override;
		bool IsPlaying() const override;
		bool IsPaused() const override;
		bool SetVolume(double volume) override;
		double Volume() const override;

		/**
		* @brief �V�����C���X�^���X�Ō��݂̃C���X�^���X��u��������
		* @param src �u���������̃C���X�^���X
		* @return ����
		*/
		bool Replace(std::shared_ptr<SE> src);

		/**
		* @brief �C���[�W��ID��ݒ肷��
		*/
		virtual void SetIdentify(const String& id);

		/**
		* @brief �C���[�W��ID���擾����
		* @return �C���[�W��ID
		*/
		virtual String Identify() const;

	private:
		std::shared_ptr<uint32_t> m_handle;

		String m_id;
	};
}

#endif
