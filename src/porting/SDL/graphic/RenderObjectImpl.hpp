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

		inline const stVertex* GetVertex() const
		{
			return m_vertex;
		}

		inline const GLushort* GetIndex() const
		{
			return m_index;
		}

		inline size_t GetVertexCount() const
		{
			return sizeof(m_vertex) / sizeof(m_vertex[0]);
		}

		inline size_t GetIndexCount() const
		{
			return sizeof(m_index) / sizeof(m_index[0]);
		}

	private:

		friend class SpriteRenderer;

		stVertex m_vertex[4] = {};		/// 頂点配列
		GLushort m_index[6] = { 0, 1, 2, 1, 2, 3 };		/// インデックス配列
	};

	class LineRenderer::Impl final
	{
	public:

		inline const stVertexSolid* GetVertex() const
		{
			return m_vertex.data();
		}

		inline const GLushort* GetIndex() const
		{
			return m_index.data();
		}

		inline size_t GetVertexCount() const
		{
			return m_vertex.size();
		}

		inline size_t GetIndexCount() const
		{
			return m_index.size();
		}

	private:

		friend class LineRenderer;

		std::vector<stVertexSolid> m_vertex;		/// 頂点配列
		std::vector<GLushort> m_index;				/// インデックス配列
	};

	class CircleRenderer::Impl final
	{
	public:

		inline const stVertexSolid* GetVertex() const
		{
			return m_vertex.data();
		}

		inline const GLushort* GetIndex() const
		{
			return m_index.data();
		}

		inline size_t GetVertexCount() const
		{
			return m_vertex.size();
		}

		inline size_t GetIndexCount() const
		{
			return m_index.size();
		}

	private:

		friend class CircleRenderer;

		std::vector<stVertexSolid> m_vertex;		/// 頂点配列
		std::vector<GLushort> m_index;				/// インデックス配列
	};
}

#endif
