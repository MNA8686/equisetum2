#include "system/Exception.hpp"
#include "system/Logger.h"
#include "graphic/Sprite.hpp"
#include <algorithm>

namespace Equisetum2
{
	std::shared_ptr<Sprite> Sprite::CreateFromTexture(std::shared_ptr<Texture> texture)
	{
		auto ptr = std::make_shared<Sprite>();
		ptr->m_texture = texture;

		stSpriteAnimAtlas atlas;
		atlas.m_point = { 0, 0 };
		atlas.m_srcSize = { (int32_t)texture->Width(), (int32_t)texture->Height() };
		atlas.m_pivot = { 0.5f, 0.5f };

		ptr->m_vAnimAtlas.push_back(atlas);

		return ptr;
	}

	bool Sprite::SetAnimAtlas(const std::vector<stSpriteAnimAtlas>& vAnimAtlas)
	{
		m_vAnimAtlas = vAnimAtlas;
		m_tags.clear();
		return true;
	}

	bool Sprite::SetTag(const String & tag, int32_t index)
	{
		if (index < 0 || index >= static_cast<int32_t>(m_vAnimAtlas.size()))
		{
			return false;
		}

		m_tags.push_back({ tag, index });

		std::sort(std::begin(m_tags), std::end(m_tags), [](stTags& a, stTags& b)->bool {
			return  a.index < b.index;
		});

		return true;
	}

	const std::vector<Sprite::stTags>& Sprite::GetTags() const
	{
		return m_tags;
	}

	int32_t Sprite::GetAtlasSizeByTagIndex(int32_t tagIndex) const
	{
		// -1を指定された場合は全体のサイズを返す
		if (tagIndex == -1)
		{
			return static_cast<int32_t>(m_vAnimAtlas.size());
		}

		// 妙な範囲を指定されていたらエラー
		if (tagIndex < 0 ||
			tagIndex >= static_cast<int32_t>(m_tags.size()))
		{
			return -1;
		}

		int32_t index = m_tags[tagIndex].index;

		if (tagIndex + 1 == static_cast<int32_t>(m_tags.size()))
		{
			return static_cast<int32_t>(m_vAnimAtlas.size()) - index;
		}

		return m_tags[tagIndex + 1].index - index;
	}

	const stSpriteAnimAtlas* Sprite::GetAtlas(int32_t num) const
	{
		if (num < 0 || num >= static_cast<int32_t>(m_vAnimAtlas.size()))
		{
			return nullptr;
		}

		return &m_vAnimAtlas[num];
	}

	int32_t Sprite::ToAtlasNumWithTagIndex(int32_t tagIndex, int32_t num) const
	{
		// -1を指定された場合はそのままnumを返す
		if (tagIndex == -1)
		{
			return num;
		}

		if (tagIndex < 0 ||
			tagIndex >= static_cast<int32_t>(m_tags.size()) ||
			num >= GetAtlasSizeByTagIndex(tagIndex))
		{
			return -1;
		}

		return m_tags[tagIndex].index + num;
	}

	int32_t Sprite::TagToInt(const String & tag) const
	{
		int32_t tagIndex = -1;

		auto findTag = std::find_if(m_tags.begin(), m_tags.end(), [&tag, &tagIndex](stTags tags) {
			tagIndex++;
			return tags.tag == tag;
		});
		
		if (findTag == m_tags.end())
		{
			return -1;
		}

		return tagIndex;
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
