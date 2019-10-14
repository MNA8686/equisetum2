#include "ResourceMapper.hpp"

using namespace Equisetum2;

stMappedResource * ResourceMapper::Map(int32_t id)
{
	if (id < 0)
	{
		return nullptr;
	}

	if (id >= static_cast<int32_t>(m_vMapped.size()))
	{
		m_vMapped.resize(id + 1);
	}

	return &m_vMapped[id];
}

void ResourceMapper::Unmap(int32_t id)
{
	if (id >= 0 &&
		id < static_cast<int32_t>(m_vMapped.size()))
	{
		m_vMapped[id] = {};
	}
}

void ResourceMapper::Reset()
{
	m_vMapped.clear();
}
