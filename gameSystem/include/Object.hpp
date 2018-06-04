#if !defined(_EQOBJECT_H_)
#define _EQOBJECT_H_

#include "Equisetum2.h"
using namespace Equisetum2;

#include <cereal/types/base_class.hpp>
#include "Node.hpp"
#include "AssetManager.hpp"
#include "Sprite.hpp"
#include "Script.hpp"
//#include "FixedDec.hpp"

/**
  �A�Z�b�g�Ǘ��\����
*/
class Script;
struct stAsset
{
	std::vector<std::shared_ptr<Sprite>> m_sprite;
	std::vector<std::shared_ptr<BGM>> m_bgm;
	std::vector<std::shared_ptr<SE>> m_se;
	std::vector<std::shared_ptr<Script>> m_script;

	// �A�Z�b�g��ID�݂̂�ۑ����Ă����A���X�g�A���ɂ̓��[�h������
	// (�X�N���v�g�͏���)
	template<class Archive>
	void save(Archive & archive) const
	{
		// �X�v���C�g
		{
			std::vector<std::string> spriteID;
			for (auto& sprite : m_sprite)
			{
				spriteID.push_back(sprite ? sprite->Identify() : "");
			}
			archive(CEREAL_NVP(spriteID));
		}

		// BGM
		{
			std::vector<std::string> bgmID;
			for (auto& p : m_bgm)
			{
				//					bgmID.push_back(p ? p->Identify() : "");
			}
			archive(CEREAL_NVP(bgmID));
		}

		// SE
		{
			std::vector<std::string> seID;
			for (auto& p : m_se)
			{
				seID.push_back(p ? p->Identify() : "");
			}
			archive(CEREAL_NVP(seID));
		}

		// �X�N���v�g
		{
			archive(CEREAL_NVP(m_script));
		}
	}

	template<class Archive>
	void load(Archive & archive)
	{
		// �X�v���C�g
		{
			std::vector<std::string> spriteID;
			archive(CEREAL_NVP(spriteID));

			int index = 0;
			m_sprite.clear();
			for (auto& id : spriteID)
			{
				m_sprite.push_back(id.empty() ? nullptr : Singleton<AssetManager>::GetInstance()->Load<Sprite>(id));
				index++;
			}
		}

		// BGM
		{
			std::vector<std::string> bgmID;
			archive(CEREAL_NVP(bgmID));

			int index = 0;
			m_bgm.clear();
			for (auto& id : bgmID)
			{
				m_bgm.push_back(id.empty() ? nullptr : Singleton<AssetManager>::GetInstance()->Load<BGM>(id));
				index++;
			}
		}

		// SE
		{
			std::vector<std::string> seID;
			archive(CEREAL_NVP(seID));

			int index = 0;
			m_se.clear();
			for (auto& id : seID)
			{
				m_se.push_back(id.empty() ? nullptr : Singleton<AssetManager>::GetInstance()->Load<SE>(id));
				index++;
			}
		}

		// �X�N���v�g
		{
			archive(CEREAL_NVP(m_script));
		}
	}
};

class Object : public Node
{
public:

	Object();
	virtual ~Object();

	static std::shared_ptr<Object> Create(const String& name);

	const Point_t<FixedDec>& GetPos() const;
	const Point_t<FixedDec>& GetLocalPos() const;
	void SetPos(const Point_t<FixedDec>& pos);
	void SetLocalPos(const Point_t<FixedDec>& pos);
	bool GetRelativeParent() const;
	void SetRelativeParent(bool on);


protected:

private:

	// --- serialize begin ---
	stAsset m_asset;				/// �A�Z�b�g�Ǘ��\����
	Point_t<FixedDec> m_pos;		/// ���[���h���W
	Point_t<FixedDec> m_localPos;	/// �e�Ƃ̑��΍��W�B�e�����Ȃ����A�܂��͐e�ɒǏ]���Ȃ�����m_pos�Ɠ����B
	bool m_relativeParent = true;	/// �e�̍��W�ɒǏ]���邩�ǂ���
//	int32_t m_angle = 0;
	bool m_active = true;			/// false�̏ꍇ�A�X�N���v�g�Ȃǂ��Ăяo����Ȃ�
	bool m_visible = true;			/// false�̏ꍇ�A�����_�����O�ΏۂƂȂ�Ȃ�
	// --- serialize end ---

	/// �q�ɐe�̍��W�ړ��𔽉f������
	void SetPosForChild();

public:

	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(cereal::base_class<Node>(this));
		archive(CEREAL_NVP(m_asset));
		archive(CEREAL_NVP(m_pos));
		archive(CEREAL_NVP(m_localPos));
		archive(CEREAL_NVP(m_relativeParent));
		archive(CEREAL_NVP(m_active));
		archive(CEREAL_NVP(m_visible));
	}
};

#include <cereal/types/polymorphic.hpp>
CEREAL_REGISTER_TYPE(Object);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Node, Object)

#endif
