#if !defined(_ASSETMAPPER_H_)
#define _ASSETMAPPER_H_

#include "Equisetum2.h"
using namespace Equisetum2;

/**
アセット管理構造体
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
	friend Singleton<AssetMapper>;	// シングルトンからインスタンスを作成してもらえるようにする

	stAsset* Map(int32_t id);
	void Unmap(int32_t id);

private:
	std::vector<stAsset> m_vAsset;
};

#endif
