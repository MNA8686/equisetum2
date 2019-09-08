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
	int32_t m_rendererIndex = -1;
};

class LineRendererContainer
{
public:
	LineRendererContainer() = default;
	~LineRendererContainer() = default;

	bool BindObject(Object* obj);
	LineRenderer* Ref();

	LineRenderer* operator->();

private:
	NodeHandler m_nodeHandler;
	int32_t m_rendererIndex = -1;
};

class RectRendererContainer
{
public:
	RectRendererContainer() = default;
	~RectRendererContainer() = default;

	bool BindObject(Object* obj);
	RectRenderer* Ref();

	RectRenderer* operator->();

private:
	NodeHandler m_nodeHandler;
	int32_t m_rendererIndex = -1;
};

class CircleRendererContainer
{
public:
	CircleRendererContainer() = default;
	~CircleRendererContainer() = default;

	bool BindObject(Object* obj);
	CircleRenderer* Ref();

	CircleRenderer* operator->();

private:
	NodeHandler m_nodeHandler;
	int32_t m_rendererIndex = -1;
};

#endif
