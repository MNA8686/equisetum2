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
		Type m_type = Type::EMPTY;
		GLint m_programID = 0;
		std::shared_ptr<stShaderCache> m_vertexCache;
		std::shared_ptr<stShaderCache> m_fragmentCache;
	};

	// 頂点定義
	typedef struct
	{
		GLfloat vertices[2];
		GLfloat texCoords[2];
		GLubyte colors[4];
	}stVertex;

	// 頂点定義
	typedef struct
	{
		GLfloat vertices[2];
		GLubyte colors[4];
	}stVertexSolid;

	class Renderer::Impl final
	{
	public:

		std::shared_ptr<SDL_Window> m_attachedWindow;		// このレンダラに関連付けされているウィンドウ
		std::shared_ptr<SDL_GLContext> m_GLContext;			// OpenGLコンテキスト

		bool SelectProgram(Type type);
		void SetProjection(int w, int h);

		typedef struct
		{
			// VBO
			GLuint m_VBO[2] = {};

			// 頂点配列
			static const int VBO_SIZE = 65536;
			stVertex m_vertex[VBO_SIZE] = {};
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
			GLuint m_VBO[2] = {};

			// 頂点配列
			static const int VBO_SIZE = 8192;
			stVertexSolid m_vertex[VBO_SIZE] = {};
			size_t m_filledVertexNum = 0;

			// インデックス配列
			static const int INDEX_VBO_SIZE = VBO_SIZE;
			GLushort m_index[INDEX_VBO_SIZE] = {};
			size_t m_filledIndexNum = 0;
		}LineContext;
		LineContext m_lineContext;

	private:

		// プロジェクションマトリックス
		GLfloat m_projection[4][4];

		// 現在設定されているプログラム
		Type m_currentProgram = Type::EMPTY;

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
