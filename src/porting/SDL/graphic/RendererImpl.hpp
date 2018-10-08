#if !defined(_EQRENDERERIMPL_H_)
#define _EQRENDERERIMPL_H_

#include "SDL.h"
#include "graphic/WindowCompatImpl.inl"
#include "graphic/Renderer.hpp"
#include "glew.h"

namespace Equisetum2
{
	// シェーダ種類
	enum class ShaderKind : int
	{
		Vertex = 0,
		Fragment
	};

	// シェーダキャッシュ
	typedef struct
	{
		ShaderKind type = ShaderKind::Vertex;
		GLint m_id = GL_INVALID_ENUM;
		const char* m_source = nullptr;
	}stShaderCache;

	// プログラムキャッシュ
	class stProgramCache
	{
	public:
		ShaderType m_type = ShaderType::EMPTY;
		GLint m_programID = 0;
		bool m_projectionDirty = true;		/// プロジェクション更新フラグ
		std::shared_ptr<stShaderCache> m_vertexCache;
		std::shared_ptr<stShaderCache> m_fragmentCache;
	};

	// 頂点定義 for スプライト
	typedef struct
	{
		GLfloat vertices[2];
		GLfloat texCoords[2];
		GLubyte colors[4];
	}stVertexSprite;

	// 頂点定義 for プリミティブ
	typedef struct
	{
		GLfloat vertices[2];
		GLubyte colors[4];
	}stVertexPrimitive;

	class Renderer::Impl final
	{
	public:

		std::shared_ptr<SDL_Window> m_attachedWindow;		// このレンダラに関連付けされているウィンドウ
		std::shared_ptr<SDL_GLContext> m_GLContext;			// OpenGLコンテキスト

		typedef struct
		{
			// VBO
			GLuint m_VBO[2] = {};

			// 頂点配列
			static const int VBO_SIZE = 65536;
			stVertexSprite m_vertex[VBO_SIZE] = {};
			size_t m_filledVertexNum = 0;

			// インデックス配列
			static const int INDEX_VBO_SIZE = VBO_SIZE * 6 / 4;
			GLushort m_index[INDEX_VBO_SIZE] = {};
			size_t m_filledIndexNum = 0;
		}SpriteContext;
		SpriteContext m_spriteContext;

		typedef struct
		{
			// VBO
			GLuint m_VBO = {};

			// レンダーオブジェクトを直接参照する
			PrimitiveRenderer* pPrimitiveRender = nullptr;
		}PrimitiveContext;
		PrimitiveContext m_primitiveContext;

	private:

		// 現在設定されているプログラム
		ShaderType m_currentProgramType = ShaderType::EMPTY;

		// ウィンドウに関連付けられたフレームバッファ
		GLuint m_framebuffer = 0;

		// プロジェクションマトリックス
		GLfloat m_projection[4][4]{};

		// シェーダをコンパイルし、キャッシュに登録する
		std::shared_ptr<stShaderCache> CompileShader(ShaderKind kind, const char* source);
		// シェーダキャッシュ
		std::list<std::shared_ptr<stShaderCache>> m_shaderCache;
		// プログラムキャッシュ
		std::list<std::shared_ptr<stProgramCache>> m_programCache;

		friend class Renderer;

	};
}

#endif
