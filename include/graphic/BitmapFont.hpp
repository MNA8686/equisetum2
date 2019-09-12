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
#if 0
		struct SerializeHint
		{
			enum class From : int32_t
			{
				FromFontManager,	// FontManager�ō��ꂽ
				FromAsset,			// �A�Z�b�g�Ƃ��ă��[�h���ꂽ
				//Raw,	// don't work
			};

			From from;
			String param;
		};
#endif

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

		const std::shared_ptr<Sprite>& GetSprite() const;
		int CodePointToAtlas(char32_t code);

	private:

		String m_id;		/// Sprite��ID
		std::shared_ptr<Sprite> m_sprite;		/// �摜�{��
		std::u32string m_codePoint;		/// �R�[�h�|�C���g�z��
	};
}

#endif
