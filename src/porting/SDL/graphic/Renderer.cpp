#include "system/Exception.hpp"
#include "system/Logger.h"

#include "graphic/Renderer.hpp"
#include "graphic/RendererImpl.hpp"
#include "graphic/TextureImpl.hpp"

namespace Equisetum2
{
	static const float inv255f = 1.0f / 255.0f;

#define STRINGIFY(x) #x

	static const char *vertexShaderSrc = R"(#version 120
		#define IN         in
		#define OUT        varying
		#define LOWP
		#define MEDIUMP
		#define HIGHP
		
		IN HIGHP   vec2 aVertex;
		uniform mat4 u_projection;
		attribute vec2 a_texCoord;
		attribute vec4 a_color;
		varying vec2 v_texCoord;
		varying vec4 v_color;
		
		void main(void)
		{
			v_texCoord = a_texCoord;
			v_color = a_color;
			gl_Position = u_projection * vec4(aVertex, 0, 1.0);
		}
	)";

	static const char *fragmentShaderSrc = R"(#version 120
		#define IN         in
		#define LOWP
		#define MEDIUMP
		#define HIGHP
		
		uniform sampler2D u_texture;
		varying vec2 v_texCoord;
		varying vec4 v_color;
		
		void main(void)
		{
			vec4 color = (v_color - 0.5) * 2;
			gl_FragColor = color + texture2D(u_texture, v_texCoord);
		}
	)";

#if 0

	static const char *vertexShaderSrc = STRINGIFY(
		\n#version 120\n
		\n#define IN         in\n
		\n#define OUT        varying\n
		\n#define LOWP\n
		\n#define MEDIUMP\n
		\n#define HIGHP\n
		\n
		IN HIGHP   vec3 aVertex; \n
		uniform mat4 u_projection; \n
		attribute vec2 a_texCoord; \n
		attribute vec4 a_color; \n
		varying vec2 v_texCoord; \n
		varying vec4 v_color; \n
		\n
		void main(void)\n
	{ \n
		v_texCoord = a_texCoord; \n
		v_color = a_color; \n
		gl_Position = u_projection * vec4(aVertex, 1.0); \n
	}\n
	);

	static const char *fragmentShaderSrc = STRINGIFY(
		\n#version 120\n
		\n#define IN         in\n
		\n#define LOWP\n
		\n#define MEDIUMP\n
		\n#define HIGHP\n
		\n
		uniform sampler2D u_texture; \n
		varying vec2 v_texCoord; \
		varying vec4 v_color; \n
		\n
		void main(void)\n
		{ \n
			vec4 color = (v_color - 0.5) * 2; \n
			gl_FragColor = color + texture2D(u_texture, v_texCoord); \n
		}\n
	);
#endif


	static std::shared_ptr<SDL_GLContext> CreateGLContext(SDL_Window* pWindow)
	{
		SDL_GLContext* pGLContext = new SDL_GLContext;
		// GLコンテキストテクスチャのデリーター作成
		auto spGLContext = std::shared_ptr<SDL_GLContext>(pGLContext,
			[](SDL_GLContext* pContext) {
			SDL_GL_DeleteContext(*pContext);
			delete pContext;
		});

		// GLコンテキスト作成
		*spGLContext = SDL_GL_CreateContext(pWindow);
		if (!*spGLContext)
		{
			return nullptr;
		}

		return spGLContext;
	}

	std::shared_ptr<Renderer> Renderer::Create()
	{
		EQ_DURING
		{
			auto& pWindow = Singleton<WindowCompat>::GetInstance()->m_Impl->GetWindowPtr();
			if (!pWindow)
			{
				EQ_THROW(u8"ウィンドウが存在しません。");
			}

			// インスタンス作成
			auto inst = std::make_shared<Renderer>();
			if (!inst)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			// インスタンス初期化
			inst->m_pImpl = std::make_shared<Renderer::Impl>();
			if (!inst->m_pImpl)
			{
				EQ_THROW(u8"インスタンスの初期化に失敗しました。");
			}

			inst->m_pImpl->m_attachedWindow = pWindow;

			auto spGLContext = CreateGLContext(pWindow.get());
			if (!spGLContext)
			{
				EQ_THROW(u8"GLコンテキストの作成に失敗しました。");
			}
			inst->m_pImpl->m_GLContext = spGLContext;

			glewInit();

#if 1
			{
				int profile_mask = 0, major = 0, minor = 0;

				if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile_mask) < 0) {
				}
				if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major) < 0) {
				}
				if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor) < 0) {
				}
				printf("profile %d, major %d, minor %d", profile_mask, major, minor);
			}
#endif

			int w, h;
			SDL_GetWindowSize(pWindow.get(), &w, &h);
			glViewport(0, 0, w, h);
			glClearColor((GLfloat)0.4, (GLfloat)0.4, (GLfloat)0.4, (GLfloat)0);
			glClear(GL_COLOR_BUFFER_BIT);

			{
				GLuint _uiProgramObject;
				GLuint		uiFragShader, uiVertShader;

				_uiProgramObject = glCreateProgram();
				uiVertShader = glCreateShader(GL_VERTEX_SHADER);
				uiFragShader = glCreateShader(GL_FRAGMENT_SHADER);

				{
					GLint glerr;

					glShaderSource(uiVertShader, 1, &vertexShaderSrc, NULL);
					glCompileShader(uiVertShader);
					{
						int logSize;
						int length;

#define MAX_SHADER_LOG_SIZE 256
						static char s_logBuffer[MAX_SHADER_LOG_SIZE];

						/* ログの長さは、最後のNULL文字も含む */
						glGetShaderiv(uiVertShader, GL_INFO_LOG_LENGTH, &logSize);

						if (logSize > 1)
						{
							glGetShaderInfoLog(uiVertShader, MAX_SHADER_LOG_SIZE, &length, s_logBuffer);
							//							fprintf(stderr, "Shader Info Log\n%s\n", s_logBuffer);
							Logger::OutputError("Shader Info Log\n%s\n", s_logBuffer);
						}
					}

					glerr = glGetError();
					if (glerr)
					{
						printf("<ERROR name=\"initWithFile\" call=\"glShaderSource\" type=\"VertShader\" glerr=\"0x%x\"/>\n", glerr);
					}
				}

				printf("frag\n");

				{
					GLint glerr;

					glShaderSource(uiFragShader, 1, &fragmentShaderSrc, NULL);
					glCompileShader(uiFragShader);
					{
						int logSize;
						int length;

#define MAX_SHADER_LOG_SIZE 256
						static char s_logBuffer[MAX_SHADER_LOG_SIZE];

						/* ログの長さは、最後のNULL文字も含む */
						glGetShaderiv(uiFragShader, GL_INFO_LOG_LENGTH, &logSize);

						if (logSize > 1)
						{
							glGetShaderInfoLog(uiFragShader, MAX_SHADER_LOG_SIZE, &length, s_logBuffer);
							//							fprintf(stderr, "Shader Info Log\n%s\n", s_logBuffer);
							Logger::OutputError("Shader Info Log\n%s\n", s_logBuffer);
						}
					}

					glerr = glGetError();
					if (glerr)
					{
						printf("<ERROR name=\"initWithFile\" call=\"glShaderSource\" type=\"FragShader\" glerr=\"0x%x\"/>\n", glerr);
					}
				}

				glAttachShader(_uiProgramObject, uiVertShader);
				glAttachShader(_uiProgramObject, uiFragShader);

				glBindAttribLocation(_uiProgramObject, 0, "aVertex");
				glBindAttribLocation(_uiProgramObject, 1, "a_texCoord");
				glBindAttribLocation(_uiProgramObject, 2, "a_color");
				//				glBindAttribLocation(_uiProgramObject, 2, "aUv1");

				{
					glLinkProgram(_uiProgramObject);
					int linked;
					glGetProgramiv(_uiProgramObject, GL_LINK_STATUS, &linked);
					printf("<LINK name=\"link\" glerr=\"0x%x\"/>\n", linked);
				}

				auto proj = glGetUniformLocation(_uiProgramObject, "u_projection");
				auto u_texture = glGetUniformLocation(_uiProgramObject, "u_texture");

				glUseProgram(_uiProgramObject);
				{
					GLint glerr = glGetError();
					if (glerr)
					{
						printf("<ERROR name=\"use\" glerr=\"0x%x\" %d/>\n", glerr, _uiProgramObject);
					}
				}

				{
					GLfloat projection[4][4];

					projection[0][0] = 2.0f / w;
					projection[0][1] = 0.0f;
					projection[0][2] = 0.0f;
					projection[0][3] = 0.0f;

					projection[1][0] = 0.0f;
					projection[1][1] = -2.0f / h;
					projection[1][2] = 0.0f;
					projection[1][3] = 0.0f;

					projection[2][0] = 0.0f;
					projection[2][1] = 0.0f;
					projection[2][2] = 0.0f;
					projection[2][3] = 0.0f;

					projection[3][0] = -1.0f;
					projection[3][1] = 1.0f;
					projection[3][2] = 0.0f;
					projection[3][3] = 1.0f;

					glUniformMatrix4fv(proj, 1, GL_FALSE, (GLfloat *)projection);
				}

				glUniform1i(u_texture, 0);

				inst->m_pImpl->_uiProgramObject = _uiProgramObject;
			}

			{
//				GLuint g_vertBuf;
//				glGenBuffers(1, &g_vertBuf);

//				inst->m_pImpl->m_vertBuf = g_vertBuf;

				auto& data = inst->m_pImpl;

				glGenBuffers(2, inst->m_pImpl->m_VBO);

				glBindBuffer(GL_ARRAY_BUFFER, data->m_VBO[0]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(data->m_vertex[0]) * data->VBO_SIZE, data->m_vertex, GL_DYNAMIC_DRAW);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, inst->m_pImpl->m_VBO[1]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(data->m_index[0]) * data->INDEX_VBO_SIZE, data->m_index, GL_STATIC_DRAW);


				// GLエラーチェック
			}

			// 1:VSYNCを待つ 0:VSYNCを待たない
			SDL_GL_SetSwapInterval(0);

			return inst;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return nullptr;
	}

#if 0
	bool Renderer::CopyQueue(const std::shared_ptr<Texture>& tex, const Rect& dst, const Rect& src)
	{
		auto vert = &m_pImpl->m_vertex[m_pImpl->m_numVertex];
		auto idx = &m_pImpl->m_index[m_pImpl->m_numIndex];

		Color color = { 128, 128, 128, 128 };
		GLfloat r = color.rgba8888.r *inv255f;
		GLfloat g = color.rgba8888.g *inv255f;
		GLfloat b = color.rgba8888.b *inv255f;
		GLfloat a = color.rgba8888.a *inv255f;

		const float divW = 1.f / (float)tex->Width();
		const float divH = 1.f / (float)tex->Height();

		// 左上
		{
			int index = 0;

			// 転送先座標
			vert[index].vertices[0] = (GLfloat)dst.x;
			vert[index].vertices[1] = (GLfloat)dst.y;
			// テクスチャ読み出し元
			vert[index].texCoords[0] = src.x * divW;
			vert[index].texCoords[1] = src.y * divH;
			// 加減算する色
			vert[index].colors[0] = r;
			vert[index].colors[1] = g;
			vert[index].colors[2] = b;
			vert[index].colors[3] = a;
		}

		// 右上
		{
			int index = 1;

			// 転送先座標
			vert[index].vertices[0] = (GLfloat)(dst.x + dst.width);
			vert[index].vertices[1] = (GLfloat)dst.y;
			// テクスチャ読み出し元
			vert[index].texCoords[0] = (src.x + src.width) * divW;
			vert[index].texCoords[1] = src.y * divH;
			// 加減算する色
			vert[index].colors[0] = r;
			vert[index].colors[1] = g;
			vert[index].colors[2] = b;
			vert[index].colors[3] = a;
		}

		// 左下
		{
			int index = 2;

			// 転送先座標
			vert[index].vertices[0] = (GLfloat)dst.x;
			vert[index].vertices[1] = (GLfloat)(dst.y + dst.height);
			// テクスチャ読み出し元
			vert[index].texCoords[0] = src.x * divW;
			vert[index].texCoords[1] = (src.y + src.height) * divH;
			// 加減算する色
			vert[index].colors[0] = r;
			vert[index].colors[1] = g;
			vert[index].colors[2] = b;
			vert[index].colors[3] = a;
		}

		// 右下
		{
			int index = 3;

			// 転送先座標
			vert[index].vertices[0] = (GLfloat)(dst.x + dst.width);
			vert[index].vertices[1] = (GLfloat)(dst.y + dst.height);
			// テクスチャ読み出し元
			vert[index].texCoords[0] = (src.x + src.width) * divW;
			vert[index].texCoords[1] = (src.y + src.height) * divH;
			// 加減算する色
			vert[index].colors[0] = r;
			vert[index].colors[1] = g;
			vert[index].colors[2] = b;
			vert[index].colors[3] = a;
		}

		idx[0] = m_pImpl->m_numVertex + 0;
		idx[1] = m_pImpl->m_numVertex + 2;
		idx[2] = m_pImpl->m_numVertex + 1;

		idx[3] = m_pImpl->m_numVertex + 1;
		idx[4] = m_pImpl->m_numVertex + 2;
		idx[5] = m_pImpl->m_numVertex + 3;

		m_pImpl->m_numVertex += 4;
		m_pImpl->m_numIndex += 6;

		return true;
	}
#endif

	bool Renderer::Copy(const std::shared_ptr<Texture>& tex, const Rect& dst, const Rect& src, int flag, int layer)
	{
		return CopyWithColor(tex, dst, src, flag, layer, { 128, 128, 128, 128 });
	}

	bool Renderer::CopyWithColor(const std::shared_ptr<Texture>& tex, const Rect& dst, const Rect& src, int flag, int layer, Color color)
	{
		auto& pWindow = Singleton<WindowCompat>::GetInstance()->m_Impl->GetWindowPtr();
//		auto& data = m_pImpl;

		SDL_GL_MakeCurrent(pWindow.get(), *(m_pImpl->m_GLContext));

		::glActiveTexture(GL_TEXTURE0);
		::glBindTexture(GL_TEXTURE_2D, *(tex->m_pImpl->m_texID));

		::glDisable(GL_DEPTH_TEST);
		::glEnable(GL_BLEND);
//		::glDisable(GL_BLEND);

		glDisable(GL_CULL_FACE);
//		glEnable(GL_CULL_FACE);

		::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//		::glBlendFunc(GL_ONE, GL_ONE);
//		::glBlendFunc(GL_SRC_ALPHA, GL_ONE);


/*
		SDL_BLENDMODE_NONE	ブレンドしない	dstRGBA = srcRGBA
			SDL_BLENDMODE_BLEND	αブレンド	dstRGB = (srcRGB * srcA) + (dstRGB * (1 - srcA))
			dstA = srcA + (dstA * (1 - srcA))
			SDL_BLENDMODE_ADD	加算ブレンド	dstRGB = (srcRGB * srcA) + dstRGB
			dstA = dstA
			*/

#if 0
		auto& vert = m_pImpl->m_vertex;

		// 左上
		{
			int index = 0;

			// 転送先座標
			vert[index].vertices[0] = (float)dst.x;
			vert[index].vertices[1] = (float)dst.y;
			// テクスチャ読み出し元
			vert[index].texCoords[0] = src.x * divW;
			vert[index].texCoords[1] = src.y * divH;
			// 加減算する色
			vert[index].colors[0] = r;
			vert[index].colors[1] = g;
			vert[index].colors[2] = b;
			vert[index].colors[3] = a;
		}

		// 右上
		{
			int index = 1;

			// 転送先座標
			vert[index].vertices[0] = (float)(dst.x + dst.width);
			vert[index].vertices[1] = (float)dst.y;
			// テクスチャ読み出し元
			vert[index].texCoords[0] = (src.x + src.width) * divW;
			vert[index].texCoords[1] = src.y * divH;
			// 加減算する色
			vert[index].colors[0] = r;
			vert[index].colors[1] = g;
			vert[index].colors[2] = b;
			vert[index].colors[3] = a;
		}

		// 左下
		{
			int index = 2;

			// 転送先座標
			vert[index].vertices[0] = (float)dst.x;
			vert[index].vertices[1] = (float)(dst.y + dst.height);
			// テクスチャ読み出し元
			vert[index].texCoords[0] = src.x * divW;
			vert[index].texCoords[1] = (src.y + src.height) * divH;
			// 加減算する色
			vert[index].colors[0] = r;
			vert[index].colors[1] = g;
			vert[index].colors[2] = b;
			vert[index].colors[3] = a;
		}

		// 右下
		{
			int index = 3;

			// 転送先座標
			vert[index].vertices[0] = (float)(dst.x + dst.width);
			vert[index].vertices[1] = (float)(dst.y + dst.height);
			// テクスチャ読み出し元
			vert[index].texCoords[0] = (src.x + src.width) * divW;
			vert[index].texCoords[1] = (src.y + src.height) * divH;
			// 加減算する色
			vert[index].colors[0] = r;
			vert[index].colors[1] = g;
			vert[index].colors[2] = b;
			vert[index].colors[3] = a;
		}

		float offset = 40.f;
		// 左上
		{
			int index = 0 + 4;

			// 転送先座標
			vert[index].vertices[0] = (float)dst.x + offset;
			vert[index].vertices[1] = (float)dst.y + offset;
			// テクスチャ読み出し元
			vert[index].texCoords[0] = src.x * divW;
			vert[index].texCoords[1] = src.y * divH;
			// 加減算する色
			vert[index].colors[0] = r;
			vert[index].colors[1] = g;
			vert[index].colors[2] = b;
			vert[index].colors[3] = a;
		}

		// 右上
		{
			int index = 1 + 4;

			// 転送先座標
			vert[index].vertices[0] = (float)(dst.x + dst.width) + offset;
			vert[index].vertices[1] = (float)dst.y + offset;
			// テクスチャ読み出し元
			vert[index].texCoords[0] = (src.x + src.width) * divW;
			vert[index].texCoords[1] = src.y * divH;
			// 加減算する色
			vert[index].colors[0] = r;
			vert[index].colors[1] = g;
			vert[index].colors[2] = b;
			vert[index].colors[3] = a;
		}

		// 左下
		{
			int index = 2 + 4;

			// 転送先座標
			vert[index].vertices[0] = (float)dst.x + offset;
			vert[index].vertices[1] = (float)(dst.y + dst.height) + offset;
			// テクスチャ読み出し元
			vert[index].texCoords[0] = src.x * divW;
			vert[index].texCoords[1] = (src.y + src.height) * divH;
			// 加減算する色
			vert[index].colors[0] = r;
			vert[index].colors[1] = g;
			vert[index].colors[2] = b;
			vert[index].colors[3] = a;
		}

		// 右下
		{
			int index = 3 + 4;

			// 転送先座標
			vert[index].vertices[0] = (float)(dst.x + dst.width) + offset;
			vert[index].vertices[1] = (float)(dst.y + dst.height) + offset;
			// テクスチャ読み出し元
			vert[index].texCoords[0] = (src.x + src.width) * divW;
			vert[index].texCoords[1] = (src.y + src.height) * divH;
			// 加減算する色
			vert[index].colors[0] = r;
			vert[index].colors[1] = g;
			vert[index].colors[2] = b;
			vert[index].colors[3] = a;
		}

		m_pImpl->m_index[0] = 0;
		m_pImpl->m_index[1] = 1;
		m_pImpl->m_index[2] = 2;

		m_pImpl->m_index[3] = 1;
		m_pImpl->m_index[4] = 2;
		m_pImpl->m_index[5] = 3;

		m_pImpl->m_index[0 + 6] = 0 + 4;
		m_pImpl->m_index[1 + 6] = 1 + 4;
		m_pImpl->m_index[2 + 6] = 2 + 4;

		m_pImpl->m_index[3 + 6] = 1 + 4;
		m_pImpl->m_index[4 + 6] = 2 + 4;
		m_pImpl->m_index[5 + 6] = 3 + 4;
#endif

		glBindBuffer(GL_ARRAY_BUFFER, m_pImpl->m_VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_pImpl->m_vertex[0]) * m_pImpl->m_numVertex, m_pImpl->m_vertex, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pImpl->m_VBO[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_pImpl->m_index[0]) * m_pImpl->m_numIndex, m_pImpl->m_index, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(m_pImpl->m_vertex[0]), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(m_pImpl->m_vertex[0]), (const void*)(2 * sizeof(GLfloat)));
//		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(m_pImpl->m_vertex[0]), (const void*)(4 * sizeof(GLfloat)));
//		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(m_pImpl->m_vertex[0]), (const void*)(4 * sizeof(GLubyte)));
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(m_pImpl->m_vertex[0]), (const void*)(4 * sizeof(GLfloat)));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glDrawElements(GL_TRIANGLES, (GLsizei)m_pImpl->m_numIndex, GL_UNSIGNED_SHORT, (GLvoid*)0);


		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		m_pImpl->m_numVertex = 0;
		m_pImpl->m_numIndex = 0;

		return true;
	}

	bool Renderer::Fill(const Rect& dst, int flag, int layer, Color color)
	{
		return false;
	}

	bool Renderer::Clear(const Color& color)
	{
		auto& pWindow = Singleton<WindowCompat>::GetInstance()->m_Impl->GetWindowPtr();

		SDL_GL_MakeCurrent(pWindow.get(), *(m_pImpl->m_GLContext));

		glClearColor(color.rgba8888.r * inv255f,
					color.rgba8888.g * inv255f,
					color.rgba8888.b * inv255f,
					color.rgba8888.a * inv255f);
		glClear(GL_COLOR_BUFFER_BIT);

		return true;
	}

	bool Renderer::Present(bool waitVsync)
	{
		auto& pWindow = Singleton<WindowCompat>::GetInstance()->m_Impl->GetWindowPtr();
		SDL_GL_MakeCurrent(pWindow.get(), *(m_pImpl->m_GLContext));

		SDL_GL_SwapWindow(pWindow.get());
		return true;
	}

}

