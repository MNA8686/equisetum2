#if !defined(_EQRENDERERCONTAINER_H_)
#define _EQRENDERERCONTAINER_H_

#include "Equisetum2.h"
using namespace Equisetum2;

#include "Object.hpp"

class SpriteRendererContainer
{
public:
	SpriteRendererContainer();
	~SpriteRendererContainer() = default;

	SpriteRenderer* Ref();

	SpriteRenderer* operator->();

private:
	NodeHandler m_nodeHandler;
	int32_t m_rendererIndex = -1;
};

class AnimationContainer
{
public:
	AnimationContainer();
	~AnimationContainer() = default;

	bool SetAnimation(int32_t assetAnimation);
	bool Inc(int32_t delta);
#if 0
	SpriteRenderer* Ref();

	SpriteRenderer* operator->();
#endif

private:
	NodeHandler m_nodeHandler;
	int32_t m_assetAnimation = -1;
	int32_t m_count = 0;

	SpriteRendererContainer m_sprite;
};

class LineRendererContainer
{
public:
	LineRendererContainer();
	~LineRendererContainer() = default;

	LineRenderer* Ref();

	LineRenderer* operator->();

private:
	NodeHandler m_nodeHandler;
	int32_t m_rendererIndex = -1;
};

class RectRendererContainer
{
public:
	RectRendererContainer();
	~RectRendererContainer() = default;

	RectRenderer* Ref();

	RectRenderer* operator->();

private:
	NodeHandler m_nodeHandler;
	int32_t m_rendererIndex = -1;
};

class CircleRendererContainer
{
public:
	CircleRendererContainer();
	~CircleRendererContainer() = default;

	CircleRenderer* Ref();

	CircleRenderer* operator->();

private:
	NodeHandler m_nodeHandler;
	int32_t m_rendererIndex = -1;
};

class TextRendererContainer
{
public:
	TextRendererContainer();
	~TextRendererContainer() = default;

	TextRenderer* Ref();

	TextRenderer* operator->();

private:
	NodeHandler m_nodeHandler;
	int32_t m_rendererIndex = -1;
};
#endif
