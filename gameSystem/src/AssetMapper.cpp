#include "AssetMapper.hpp"

using namespace Equisetum2;

stAsset * AssetMapper::Map(int32_t id)
{
	if (id < 0)
	{
		return nullptr;
	}

	if (id >= static_cast<int32_t>(m_vAsset.size()))
	{
		m_vAsset.resize(id + 1);
	}

	return &m_vAsset[id];
}

void AssetMapper::Unmap(int32_t id)
{
	if (id >= 0 &&
		id < static_cast<int32_t>(m_vAsset.size()))
	{
		m_vAsset[id] = {};
	}
}
