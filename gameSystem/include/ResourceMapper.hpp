#if !defined(_RESOURCEMAPPER_H_)
#define _RESOURCEMAPPER_H_

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

struct stMappedResource
{
	stAsset asset;
	std::vector<std::shared_ptr<RenderObject>> renderObject;	/// レンダーオブジェクト配列
};

class ResourceMapper
{
public:
	friend Singleton<ResourceMapper>;	// シングルトンからインスタンスを作成してもらえるようにする

	stMappedResource* Map(int32_t id);
	void Unmap(int32_t id);

private:
	std::vector<stMappedResource> m_vMapped;
};

#endif
