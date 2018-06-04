#if !defined(_EQRENDERERIMPL_H_)
#define _EQRENDERERIMPL_H_

#include "SDL.h"
#include "graphic/WindowCompatImpl.inl"
#include "graphic/Renderer.hpp"
#include "glew.h"

namespace Equisetum2
{
	typedef struct
	{
		GLfloat vertices[2];
		GLfloat texCoords[2];
		GLfloat colors[4];
	}stVertex;

	class Renderer::Impl final
	{
	public:

		static const int VBO_SIZE = 65536;
		static const int INDEX_VBO_SIZE = VBO_SIZE * 6 / 4;

//		std::shared_ptr<SDL_GLContext> CreateGLContext(SDL_Window* pWindow);

		std::shared_ptr<SDL_Window> m_attachedWindow;		// このレンダラに関連付けされているウィンドウ
		std::shared_ptr<SDL_GLContext> m_GLContext;			// OpenGLコンテキスト

		GLuint _uiProgramObject;
//		GLuint m_vertBuf;
		GLuint m_VBO[2];

//		std::vector<stVertex> m_vertex;
		stVertex m_vertex[VBO_SIZE] = {};
		GLushort m_index[INDEX_VBO_SIZE] = {};

		int m_numVertex = 0;
		int m_numIndex = 0;
	};
}

#endif
