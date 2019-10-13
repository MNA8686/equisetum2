#if !defined(_EQBITMAPFONT_H_)
#define _EQBITMAPFONT_H_

#include "type/String.h"
#include "graphic/Sprite.hpp"
#include <memory>

namespace Equisetum2
{
	class BitmapFont
	{
	public:

		struct SerializeHint
		{
			enum class From : int32_t
			{
				Empty,
				FontManager,	// FontManager�ō��ꂽ
				Asset,			// �A�Z�b�g�Ƃ��ă��[�h���ꂽ
				//Raw,	// don't work
			};

			From from = From::Empty;
			String id;
			Color color{};
			Size maxSize;
		};

		BitmapFont() = default;
		virtual ~BitmapFont() = default;

		static std::shared_ptr<BitmapFont> CreateFromSprite(const std::shared_ptr<Sprite>& sprite, const String& codePoint);

		/**
		* @brief �V�����C���X�^���X�Ō��݂̃C���X�^���X��u��������
		* @param src �u���������̃C���X�^���X
		* @return ����
		*/
		virtual bool MoveFrom(std::shared_ptr<BitmapFont>&& src);

		/**
		* @brief �C���X�^���X��ID��ݒ肷��
		*/
		virtual void SetIdentify(const String& id);

		/**
		* @brief �C���X�^���X��ID���擾����
		* @return �C���X�^���X��ID
		*/
		virtual String Identify() const;

		/**
		* @brief �R�[�h�|�C���g��������擾����
		* @return �R�[�h�|�C���g������
		*/
		virtual String CodePoint() const;

		/**
		* @brief �ݒ肳��Ă���X�v���C�g���擾����
		* @return �ݒ肳��Ă���X�v���C�g
		*/
		const std::shared_ptr<Sprite>& GetSprite() const;

		/**
		* @brief �R�[�h�|�C���g���X�v���C�g�A�g���X�̉��ԖڂɊ��蓖�Ă��Ă��邩��Ԃ�
		* @return �A�g���X�ԍ�
		*/
		int CodePointToAtlas(char32_t code);

		/**
		* @brief �V���A���C�Y�ɕK�v�ȃq���g��ݒ肷��
		* @param hint �ݒ肷��q���g
		* @return �Ȃ� 
		*/
		virtual void SetHint(const SerializeHint& hint);

		/**
		* @brief �V���A���C�Y�ɕK�v�ȃq���g���擾����
		* @return �q���g
		*/
		virtual SerializeHint GetHint(void) const;

	private:

		String m_id;		/// Sprite��ID
		std::shared_ptr<Sprite> m_sprite;		/// �摜�{��
		std::u32string m_codePoint;		/// �R�[�h�|�C���g�z��
		SerializeHint m_hint;			/// �V���A���C�Y�p�q���g
	};
}

#endif
