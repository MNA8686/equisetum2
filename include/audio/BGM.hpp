/**
* @file BGM.hpp
* @brief BGM�Đ��N���X
* @author MNA
* @date 2017/09/07 �V�K�쐬
*/
#if !defined(_EQBGM_H_)
#define _EQBGM_H_

#include "audio/ISound.hpp"
#include "stream/IStream.h"
#include <memory>

namespace Equisetum2
{
	/**
	* BGM���Đ����邽�߂̃N���X�B
	*/
	class BGM : public ISound
	{
	public:
		/**
		* @brief �X�g���[������BGM���X�g���[�~���O�Đ�����C���X�^���X���쐬����
		* @param stream ���̓X�g���[��(�ǂݏo���A�V�[�N�������K�v)
		* @return �쐬�����C���X�^���X
		*/
		static std::shared_ptr<BGM> CreateFromStream(const std::shared_ptr<IStream> stream);

		bool Play(bool loop) override;
		bool Stop() override;
		void Pause() override;
		void Resume() override;
		bool IsPlaying() const override;
		bool IsPaused() const override;
		bool SetVolume(double volume) override;
		double Volume() const override;
		bool SetPos(double pos);

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
		std::shared_ptr<uint32_t> m_id;
		String m_identify;
	};
}

#endif
