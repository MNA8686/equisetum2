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

		inline const stVertexSprite* GetVertex() const
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

		stVertexSprite m_vertex[4] = {};		/// 頂点配列
		GLushort m_index[6] = { 0, 1, 2, 1, 2, 3 };		/// インデックス配列
	};

	class TextRenderer::Impl final
	{
	public:

		inline const stVertexSprite* GetVertex() const
		{
			return m_vertex.data();
		}

		inline const GLushort* GetIndex() const
		{
			return m_index.data();
		}

		inline size_t GetVertexCount() const
		{
			return m_vertexSize;
		}

		inline size_t GetIndexCount() const
		{
			return m_indexSize;
		}

	private:

		friend class TextRenderer;

		std::vector<stVertexSprite> m_vertex;	/// 頂点配列
		int32_t m_vertexSize = 0;		/// vectorにpush_backするとdebug不能な遅さになるため、vectorはただの可変長配列として扱う

		std::vector<GLushort> m_index;	/// 頂点配列
		int32_t m_indexSize = 0;		/// vectorにpush_backするとdebug不能な遅さになるため、vectorはただの可変長配列として扱う
	};

	class PrimitiveRenderer::Impl final
	{
	public:

		inline const stVertexPrimitive* GetVertex() const
		{
			return m_vertex.data();
		}

		inline size_t GetVertexCount() const
		{
			return m_vertexSize;
		}

		inline BlendMode GetBlendMode() const
		{
			return m_blend;
		}

		inline bool GetSolid() const
		{
			return m_solid;
		}

	private:

		friend class PrimitiveRenderer;
		friend class LineRenderer;
		friend class RectRenderer;
		friend class CircleRenderer;

		std::vector<stVertexPrimitive> m_vertex;	/// 頂点配列
		int32_t m_vertexSize = 0;		/// vectorにpush_backするとdebug不能な遅さになるため、vectorはただの可変長配列として扱う

		BlendMode m_blend = BlendMode::None;		/// ブレンドモード
		bool m_solid = false;		/// 塗りつぶしモード
	};
}

#endif
