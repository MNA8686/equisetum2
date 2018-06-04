#if !defined(_EQSPRITE_H_)
#define _EQSPRITE_H_

//#include "Equisetum2.h"
#include "type/String.h"
#include "type/Point.h"
#include "type/Rect.hpp"
#include "graphic/Texture.hpp"
#include <memory>

namespace Equisetum2
{
	typedef struct
	{
	public:
		Point m_point;		// �J�n���W
		Size m_srcSize;		// �]�����T�C�Y
		PointF m_pivot;		// ���S���W
	}stSpriteAnimAtlas;

	class Sprite
	{
	public:
		static const Color ZERO;

		Sprite() = default;
		virtual ~Sprite() = default;

		static std::shared_ptr<Sprite> CreateFromTexture(std::shared_ptr<Texture> texture);
		bool SetAnimAtlas(const std::vector<stSpriteAnimAtlas>& vAnimAtlas);

		const stSpriteAnimAtlas& GetAtlas(int32_t num);

		/**
		* @brief �V�����C���X�^���X�Ō��݂̃C���X�^���X��u��������
		* @param src �u���������̃C���X�^���X
		* @return ����
		*/
		virtual bool MoveFrom(std::shared_ptr<Sprite>&& src);

		/**
		* @brief �C���[�W��ID��ݒ肷��
		*/
		virtual void SetIdentify(const String& id);

		/**
		* @brief �C���[�W��ID���擾����
		* @return �C���[�W��ID
		*/
		virtual String Identify() const;

		const std::shared_ptr<Texture>& GetTexture() const;
		const std::vector<stSpriteAnimAtlas>& GetAnimAtlas() const;

	private:
		std::shared_ptr<Texture> m_texture;		/// �摜�{��
		std::vector<stSpriteAnimAtlas> m_vAnimAtlas;	/// �؂�o���ʒu

		String m_id;		/// Sprite��ID
	};
}

#endif
