#if !defined(_EQRENDEROBJECTIMPL_H_)
#define _EQRENDEROBJECTIMPL_H_

#include "SDL.h"
#include "graphic/Renderer.hpp"
#include "graphic/RendererImpl.hpp"
#include "graphic/RenderObject.hpp"
#include "graphic/Sprite.hpp"
#include "glew.h"

namespace Equisetum2
{
	class SpriteRenderer::Impl final
	{
	public:
		const stVertex& GetVertex() const;
		const GLushort& GetIndex() const;

	private:
		friend class SpriteRenderer;

		stVertex m_vertex[4] = {};		/// 頂点数
		GLushort m_index[6] = {};		/// インデックス数
	};
}

#endif
