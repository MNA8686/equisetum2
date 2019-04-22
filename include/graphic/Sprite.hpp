#if !defined(_EQSPRITE_H_)
#define _EQSPRITE_H_

//#include "Equisetum2.h"
#include "type/String.h"
#include "type/Point.h"
#include "type/Rect.hpp"
#include "graphic/Texture.hpp"
#include <memory>
#include <unordered_map>

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

		typedef struct
		{
			String tag;
			int32_t index;
		}stTags;

		Sprite() = default;
		virtual ~Sprite() = default;

		static std::shared_ptr<Sprite> CreateFromTexture(std::shared_ptr<Texture> texture);
		bool SetAnimAtlas(const std::vector<stSpriteAnimAtlas>& vAnimAtlas);
		bool SetTag(const String& tag, int32_t index);
		const std::vector<stTags>& GetTags() const;
		int32_t GetAtlasSizeByTagIndex(int32_t tagIndex) const;

		const stSpriteAnimAtlas* GetAtlas(int32_t num) const;
		int32_t ToAtlasNumWithTagIndex(int32_t tagIndex, int32_t num) const;
		int32_t TagToInt(const String& tag) const;

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

		std::vector<stTags> m_tags;		/// �^�O�ƃI�t�Z�b�g

		String m_id;		/// Sprite��ID
	};
}

#endif
