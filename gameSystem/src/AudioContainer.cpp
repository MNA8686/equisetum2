#include "AudioContainer.hpp"

BgmContainer::BgmContainer()
{
	if (auto obj = Object::GetCreatingObject())
	{
		m_nodeHandler = obj->GetNodeHandler();
	}
}

bool BgmContainer::SetBGM(int32_t assetBGM)
{
	m_assetBGM = assetBGM;

	return true;
}

BGM* BgmContainer::Ref()
{
	if (auto obj = Object::GetObjectByHandler(m_nodeHandler))
	{
		return obj->GetAsset()->m_bgm[m_assetBGM].get();
	}

	return nullptr;
}

BGM* BgmContainer::operator->()
{
	return Ref();
}

//-----------------------------------------------------------------------------------------------

SeContainer::SeContainer()
{
	if (auto obj = Object::GetCreatingObject())
	{
		m_nodeHandler = obj->GetNodeHandler();
	}
}

bool SeContainer::SetSE(int32_t assetSE)
{
	m_assetSE = assetSE;

	return true;
}

SE* SeContainer::Ref()
{
	if (auto obj = Object::GetObjectByHandler(m_nodeHandler))
	{
		return obj->GetAsset()->m_se[m_assetSE].get();
	}

	return nullptr;
}

SE* SeContainer::operator->()
{
	return Ref();
}



