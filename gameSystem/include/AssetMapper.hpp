#if !defined(_ASSETMAPPER_H_)
#define _ASSETMAPPER_H_

#include "Equisetum2.h"
using namespace Equisetum2;

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
};

class AssetMapper
{
public:
	friend Singleton<AssetMapper>;	// �V���O���g������C���X�^���X���쐬���Ă��炦��悤�ɂ���

	stAsset* Map(int32_t id);
	void Unmap(int32_t id);

private:
	std::vector<stAsset> m_vAsset;
};

#endif
