#if !defined(_EQRENDERERCONTAINER_H_)
#define _EQRENDERERCONTAINER_H_

#include "Equisetum2.h"
using namespace Equisetum2;

#include "Object.hpp"

class SpriteRendererContainer
{
public:
	SpriteRendererContainer() = default;
	~SpriteRendererContainer() = default;

	bool BindObject(Object* obj);
	SpriteRenderer* Ref();

	SpriteRenderer* operator->();

private:
	NodeHandler m_nodeHandler;
	int32_t m_spriteRendererIndex = -1;
};

#endif
