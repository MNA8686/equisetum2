#include "system/Exception.hpp"
#include "system/Logger.h"
#include "graphic/Sprite.hpp"

namespace Equisetum2
{
	std::shared_ptr<Sprite> Sprite::CreateFromTexture(std::shared_ptr<Texture> texture)
	{
		auto ptr = std::make_shared<Sprite>();
		ptr->m_texture = texture;

		return ptr;
	}

	bool Sprite::SetAnimAtlas(const std::vector<stSpriteAnimAtlas>& vAnimAtlas)
	{
		m_vAnimAtlas = vAnimAtlas;
		return true;
	}

	const stSpriteAnimAtlas& Sprite::GetAtlas(int32_t num) const
	{
		// TODO:何も設定されていないときのことも考える

		if (num < 0)
		{
			num = 0;
		}
		else if (num >= m_vAnimAtlas.size())
		{
			num = m_vAnimAtlas.size() - 1;
		}

		return m_vAnimAtlas[num];
	}

	bool Sprite::MoveFrom(std::shared_ptr<Sprite>&& src)
	{
		m_id = std::move(src->m_id);
		m_texture->MoveFrom(std::move(src->m_texture));
		m_vAnimAtlas = std::move(src->m_vAnimAtlas);

		return true;
	}

	const std::shared_ptr<Texture>& Sprite::GetTexture() const
	{
		return m_texture;
	}

	const std::vector<stSpriteAnimAtlas>& Sprite::GetAnimAtlas() const
	{
		return m_vAnimAtlas;
	}

	void Sprite::SetIdentify(const String& id)
	{
		m_id = id;
	}

	String Sprite::Identify() const
	{
		return m_id;
	}

	const Color Sprite::ZERO{ 128, 128, 128, 128 };
}
