#if !defined(_EQAUDIOCONTAINER_H_)
#define _EQAUDIOCONTAINER_H_

#include "Equisetum2.h"
using namespace Equisetum2;

#include "Object.hpp"

class BgmContainer
{
public:
	BgmContainer();
	~BgmContainer() = default;

	bool SetBGM(int32_t assetBGM);
	BGM* Ref();
	BGM* operator->();

private:
	NodeHandler m_nodeHandler;
	int32_t m_bgmIndex = -1;
	int32_t m_assetBGM = -1;
};

class SeContainer
{
public:
	SeContainer();
	~SeContainer() = default;

	bool SetSE(int32_t assetSE);
	SE* Ref();
	SE* operator->();

private:
	NodeHandler m_nodeHandler;
	int32_t m_seIndex = -1;
	int32_t m_assetSE = -1;
};

#endif
