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
	void SetDegree(int32_t degree);
	int32_t TagToInt(const String& tag) const;
	void Start(int32_t tagIndex, bool reverse=false);	/// アニメーションを開始する
	bool Inc(int32_t delta=AnimationTimeline::timePerFrame);		/// アニメーションを進める(1000で1フレーム相当)
	void Reverse();			/// アニメーションの向きを変更する
	int32_t GetTagIndex() const;
	int32_t GetLoopCount() const;
	int32_t GetAnimIndex() const;
	int32_t GetTotalTime() const;
	SpriteRenderer* GetSpriteRenderer();

private:
	NodeHandler m_nodeHandler;
	int32_t m_assetAnimation = -1;
	int32_t m_count = 0;		/// 経過時間
	int32_t m_tagIndex = -1;	/// 選択されているアニメーションの名前
	int32_t m_degree = 0;		/// 回転アニメーション用の角度
	int32_t m_loopCount = 0;	/// Update()時にアニメーションが何周したか
	int32_t m_animIndex = -1;	/// 現在表示しているアニメーションパターンのインデックス

	SpriteRendererContainer m_sprite;
	bool m_reverse = false;

	void Update();
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
