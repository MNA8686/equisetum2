#include "system/Exception.hpp"
#include "system/Logger.h"

#include "graphic/Renderer.hpp"
#include "graphic/RendererImpl.hpp"
#include "graphic/TextureImpl.hpp"
#include "graphic/RenderObject.hpp"
#include "graphic/RenderObjectImpl.hpp"
#include <algorithm>
#include <list>

#include "input/Key.h"
#include "system/System.h"

namespace Equisetum2
{
	static const float inv255f = 1.0f / 255.0f;

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
		const float inv255f = 1. / 255.;
		
		void main(void)
		{
			// 光らせたり暗くしたりフェードイン・フェードアウトするための処理。 (128, 128, 128, 128)が中央値。
			v_color = ((a_color * inv255f) - 0.5) * 2;
			// UV座標
			v_texCoord = a_texCoord;
			// 表示座標
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
			gl_FragColor = v_color + texture2D(u_texture, v_texCoord);
		}
	)";

	static const char *vertexShaderSolidSrc = R"(#version 120
		#define IN         in
		#define OUT        varying
		#define LOWP
		#define MEDIUMP
		#define HIGHP
		
		IN HIGHP   vec2 aVertex;
		uniform mat4 u_projection;
		attribute vec4 a_color;
		varying vec4 v_color;
		const float inv255f = 1. / 255.;
		
		void main(void)
		{
			v_color = a_color * inv255f;
			// 表示座標
			gl_Position = u_projection * vec4(aVertex, 0, 1.0);
		}
	)";

	static const char *fragmentShaderSolidSrc = R"(#version 120
		#define IN         in
		#define LOWP
		#define MEDIUMP
		#define HIGHP
		
		varying vec4 v_color;
		
		void main(void)
		{
			gl_FragColor = v_color;
		}
	)";

	// シェーダ定義構造体
	typedef struct
	{
		Type m_type;		// シェーダ用途タイプ
		const char* m_vertexSource;			// バーテックスシェーダのソース
		const char* m_fragmentSource;		// フラグメントシェーダのソース
	}stShaderDef;

	// シェーダ定義テーブル
	static const stShaderDef gShaderTbl[] =
	{
		{
			Type::SPRITE,
			vertexShaderSrc,
			fragmentShaderSrc
		},
		{
			Type::LINE,
			vertexShaderSolidSrc,
			fragmentShaderSolidSrc
		},
		{
			Type::CIRCLE,
			vertexShaderSolidSrc,
			fragmentShaderSolidSrc
		},
	};

	static std::shared_ptr<SDL_GLContext> CreateGLContext(SDL_Window* pWindow)
	{
		SDL_GLContext* pGLContext = new SDL_GLContext;
		// GLコンテキストテクスチャのデリーター作成
		auto spGLContext = std::shared_ptr<SDL_GLContext>(pGLContext, [](SDL_GLContext* pContext) {
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

			// レンダーキュー領域を確保
			for (int i = 0; i < LayerMax; i++)
			{
				inst->m_vRenderObject[i].reserve(1024);
			}

			// GLコンテキスト作成
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
			inst->m_pImpl->SetProjection(w, h);

			glClearColor((GLfloat)0, (GLfloat)0, (GLfloat)0, (GLfloat)0);
			glClear(GL_COLOR_BUFFER_BIT);

			// 1:VSYNCを待つ 0:VSYNCを待たない
			SDL_GL_SetSwapInterval(1);

			return inst;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return nullptr;
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

	bool Renderer::AddRenderQueue(RenderObject* pRenderObject)
	{
		auto layer = pRenderObject->GetLayer();
		if (layer < LayerMax)
		{
			m_vRenderObject[layer].push_back(pRenderObject);
			return true;
		}

		return false;
	}

	void Renderer::SortRenderQueue()
	{
		for (auto& layer : m_vRenderObject)
		{
			std::sort(std::begin(layer), std::end(layer), [](RenderObject* a, RenderObject* b)->bool {
				return  a->GetOrderInLayer() < b->GetOrderInLayer();
			});
		}
	}

	static int gDrawCallCount = 0;

	bool Renderer::Render()
	{
		auto& pWindow = Singleton<WindowCompat>::GetInstance()->m_Impl->GetWindowPtr();
		auto& spriteContext = m_pImpl->m_spriteContext;
		auto& lineContext = m_pImpl->m_lineContext;
		auto& circleContext = m_pImpl->m_circleContext;

		gDrawCallCount = 0;

		SDL_GL_MakeCurrent(pWindow.get(), *(m_pImpl->m_GLContext));

		// ソートする
		SortRenderQueue();

		// カレントステートをリセット
		m_currentStates.type = Type::EMPTY;
		m_currentStates.blend = BlendMode::None;
		m_currentStates.pTexture = nullptr;

		spriteContext.m_filledVertexNum = 0;
		spriteContext.m_filledIndexNum = 0;
		lineContext.m_filledVertexNum = 0;
		lineContext.m_filledIndexNum = 0;
		circleContext.m_filledVertexNum = 0;
		circleContext.m_filledIndexNum = 0;

		for (auto& layer : m_vRenderObject)
		{
			for (auto& renderObject : layer)
			{
				if (renderObject->GetType() == Type::SPRITE)
				{
					auto spriteRenderer = static_cast<SpriteRenderer*>(renderObject);

					// このスプライトの頂点数
					auto vertexCount = spriteRenderer->m_pImpl->GetVertexCount();
					auto blendMode = spriteRenderer->m_blend;
					auto pTexture = spriteRenderer->m_sprite->GetTexture().get();

					// 頂点配列が全て埋まっている？またはステートが変化した？
					if (m_currentStates.type != Type::SPRITE ||
						*(m_currentStates.pTexture->m_pImpl->GetTexID()) != *(pTexture->m_pImpl->GetTexID()) ||
						m_currentStates.blend != blendMode ||
						spriteContext.m_filledVertexNum + vertexCount >= spriteContext.VBO_SIZE)
					{
						// 描画を行う
						DrawCall();

						// ステートを更新
						m_currentStates.type = Type::SPRITE;
						m_currentStates.blend = blendMode;
						m_currentStates.pTexture = pTexture;
					}

					// 頂点配列を埋める
					auto vertex = spriteRenderer->m_pImpl->GetVertex();		// このスプライトの頂点バッファ
					memcpy(&spriteContext.m_vertex[spriteContext.m_filledVertexNum], vertex, sizeof(stVertex) * vertexCount);

					// インデックス配列を埋める
					auto index = spriteRenderer->m_pImpl->GetIndex();		// このスプライトのインデックスバッファ
					auto indexCount = spriteRenderer->m_pImpl->GetIndexCount();		// このスプライトのインデックス数
					for (decltype(indexCount) i = 0; i < indexCount; i++)
					{
						// 頂点の番号を変換しながらコピーする
						spriteContext.m_index[spriteContext.m_filledIndexNum + i] = static_cast<GLushort>(spriteContext.m_filledVertexNum + index[i]);
					}

					spriteContext.m_filledVertexNum += vertexCount;
					spriteContext.m_filledIndexNum += indexCount;
				}
				else if (renderObject->GetType() == Type::LINE)
				{
					auto lineRenderer = static_cast<LineRenderer*>(renderObject);

					// このスプライトの頂点数
					auto vertexCount = lineRenderer->m_pImpl->GetVertexCount();
					auto blendMode = lineRenderer->m_blend;

					if (vertexCount > 0)
					{
						// 頂点配列が全て埋まっている？またはステートが変化した？
						if (m_currentStates.type != Type::LINE ||
							m_currentStates.blend != blendMode ||
							lineContext.m_filledVertexNum + vertexCount >= lineContext.VBO_SIZE)
						{
							// 描画を行う
							DrawCall();

							// ステートを更新
							m_currentStates.type = Type::LINE;
							m_currentStates.blend = blendMode;
						}

						// 頂点配列を埋める
						auto vertex = lineRenderer->m_pImpl->GetVertex();		// このスプライトの頂点バッファ
						memcpy(&lineContext.m_vertex[lineContext.m_filledVertexNum], vertex, sizeof(stVertexSolid) * vertexCount);

						// インデックス配列を埋める
						auto index = lineRenderer->m_pImpl->GetIndex();		// このスプライトのインデックスバッファ
						auto indexCount = lineRenderer->m_pImpl->GetIndexCount();		// このスプライトのインデックス数
						for (decltype(indexCount) i = 0; i < indexCount; i++)
						{
							// 頂点の番号を変換しながらコピーする
							lineContext.m_index[lineContext.m_filledIndexNum + i] = static_cast<GLushort>(lineContext.m_filledVertexNum + index[i]);
						}

						lineContext.m_filledVertexNum += vertexCount;
						lineContext.m_filledIndexNum += indexCount;
					}
				}
				else if (renderObject->GetType() == Type::CIRCLE)
				{
					auto circleRenderer = static_cast<CircleRenderer*>(renderObject);

					// このスプライトの頂点数
					auto vertexCount = circleRenderer->m_pImpl->GetVertexCount();
					auto blendMode = circleRenderer->m_blend;

#if 1
					if (vertexCount > 0)
					{
						// 描画を行う
						DrawCall();

						// ステートを更新
						m_currentStates.type = Type::CIRCLE;
						m_currentStates.blend = blendMode;

						// 頂点配列を埋める
						auto vertex = circleRenderer->m_pImpl->GetVertex();		// このスプライトの頂点バッファ
						memcpy(&circleContext.m_vertex[circleContext.m_filledVertexNum], vertex, sizeof(stVertexSolid) * vertexCount);

						// インデックス配列を埋める
						auto index = circleRenderer->m_pImpl->GetIndex();		// このスプライトのインデックスバッファ
						auto indexCount = circleRenderer->m_pImpl->GetIndexCount();		// このスプライトのインデックス数
						for (decltype(indexCount) i = 0; i < indexCount; i++)
						{
							// 頂点の番号を変換しながらコピーする
							circleContext.m_index[circleContext.m_filledIndexNum + i] = static_cast<GLushort>(circleContext.m_filledVertexNum + index[i]);
						}

#if 1
						circleContext.m_filledVertexNum += vertexCount;
						circleContext.m_filledIndexNum += indexCount;
#endif

						// 描画を行う
						DrawCall();

						m_currentStates.type = Type::EMPTY;
					}
#endif
				}
			}
		}

		// バッファに残っているものを描画する
		DrawCall();

		// レンダーキューをクリアする
		for (auto& layer : m_vRenderObject)
		{
			layer.clear();
		}

		Logger::OutputDebug("draw call %d", gDrawCallCount);

		return true;
	}

	bool Renderer::DrawCall()
	{
		if (m_currentStates.type == Type::SPRITE)
		{
			auto& ctx = m_pImpl->m_spriteContext;

			if (m_pImpl->SelectProgram(Type::SPRITE))
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, *(m_currentStates.pTexture->m_pImpl->GetTexID()));

				glDisable(GL_DEPTH_TEST);

				switch (m_currentStates.blend)
				{
				case BlendMode::None:
					glDisable(GL_BLEND);
					break;

				case BlendMode::Blend:
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					break;

				case BlendMode::Add:
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE);
					break;
				}

				glDisable(GL_CULL_FACE);

				glBindBuffer(GL_ARRAY_BUFFER, ctx.m_VBO[0]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(ctx.m_vertex[0]) * ctx.m_filledVertexNum, ctx.m_vertex, GL_DYNAMIC_DRAW);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctx.m_VBO[1]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ctx.m_index[0]) * ctx.m_filledIndexNum, ctx.m_index, GL_STATIC_DRAW);

				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ctx.m_vertex[0]), 0);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ctx.m_vertex[0]), (const void*)(2 * sizeof(GLfloat)));
				glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(ctx.m_vertex[0]), (const void*)(4 * sizeof(GLfloat)));

				glDrawElements(GL_TRIANGLES, (GLsizei)ctx.m_filledIndexNum, GL_UNSIGNED_SHORT, (GLvoid*)0);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}

			ctx.m_filledVertexNum = 0;
			ctx.m_filledIndexNum = 0;

			gDrawCallCount++;
		}
		else if (m_currentStates.type == Type::LINE)
		{
			auto& ctx = m_pImpl->m_lineContext;

			if (m_pImpl->SelectProgram(Type::LINE))
			{
				glDisable(GL_DEPTH_TEST);

				switch (m_currentStates.blend)
				{
				case BlendMode::None:
					glDisable(GL_BLEND);
					break;

				case BlendMode::Blend:
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					break;

				case BlendMode::Add:
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE);
					break;
				}

				glDisable(GL_CULL_FACE);

				glBindBuffer(GL_ARRAY_BUFFER, ctx.m_VBO[0]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(ctx.m_vertex[0]) * ctx.m_filledVertexNum, ctx.m_vertex, GL_DYNAMIC_DRAW);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctx.m_VBO[1]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ctx.m_index[0]) * ctx.m_filledIndexNum, ctx.m_index, GL_STATIC_DRAW);

				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ctx.m_vertex[0]), 0);
				glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(ctx.m_vertex[0]), (const void*)(2 * sizeof(GLfloat)));

				glDrawArrays(GL_LINES, 0, (GLsizei)ctx.m_filledIndexNum);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

				gDrawCallCount++;
			}

			ctx.m_filledVertexNum = 0;
			ctx.m_filledIndexNum = 0;
		}
		else if (m_currentStates.type == Type::CIRCLE)
		{
			auto& ctx = m_pImpl->m_circleContext;

			if (m_pImpl->SelectProgram(Type::CIRCLE))
			{
				glDisable(GL_DEPTH_TEST);

				switch (m_currentStates.blend)
				{
				case BlendMode::None:
					glDisable(GL_BLEND);
					break;

				case BlendMode::Blend:
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					break;

				case BlendMode::Add:
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE);
					break;
				}

				glDisable(GL_CULL_FACE);

				glBindBuffer(GL_ARRAY_BUFFER, ctx.m_VBO[0]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(ctx.m_vertex[0]) * ctx.m_filledVertexNum, ctx.m_vertex, GL_DYNAMIC_DRAW);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctx.m_VBO[1]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ctx.m_index[0]) * ctx.m_filledIndexNum, ctx.m_index, GL_STATIC_DRAW);

				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ctx.m_vertex[0]), 0);
				glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(ctx.m_vertex[0]), (const void*)(2 * sizeof(GLfloat)));

				glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)ctx.m_filledIndexNum);
//				glDrawArrays(GL_LINE_STRIP/*GL_TRIANGLE_FAN*/, 0, (GLsizei)ctx.m_filledIndexNum);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

				gDrawCallCount++;
			}

			ctx.m_filledVertexNum = 0;
			ctx.m_filledIndexNum = 0;
		}

		return true;
	}

	std::shared_ptr<RenderObject> Renderer::CreateRenderObject(Type type)
	{
		std::shared_ptr<RenderObject> obj;

		switch (type)
		{
		case Type::SPRITE:
			obj = SpriteRenderer::Create(shared_from_this());
			break;
		case Type::LINE:
			obj = LineRenderer::Create(shared_from_this());
			break;
		case Type::CIRCLE:
			obj = CircleRenderer::Create(shared_from_this());
			break;
		}

		return obj;
	}


	std::shared_ptr<stShaderCache> Renderer::Impl::CompileShader(ShaderKind kind, const char* source)
	{
		EQ_DURING
		{
			// キャッシュから探し、あればそのインスタンスを返す
			for (auto& cache : m_shaderCache)
			{
				if (cache->type == kind &&
					cache->m_source == source)
				{
					return cache;
				}
			}

			// シェーダキャッシュを作る
			auto newShaderCache = std::shared_ptr<stShaderCache>(new stShaderCache, [](stShaderCache* p) {
				if (p)
				{
					if (p->m_id != GL_INVALID_ENUM)
					{
						glDeleteShader(p->m_id);
					}

					delete p;
				}
			});
			// シェーダタイプを設定
			newShaderCache->type = kind;

			// ソースからシェーダをコンパイルする
			GLuint newShader = glCreateShader(kind == ShaderKind::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
			glShaderSource(newShader, 1, &source, NULL);
			glCompileShader(newShader);

			// シェーダを保持する
			newShaderCache->m_source = source;
			newShaderCache->m_id = newShader;

			GLint logSize = 0;
			GLsizei length = 0;
			std::vector<char> vLog;

			// ログの長さは、最後のNULL文字も含む
			glGetShaderiv(newShader, GL_INFO_LOG_LENGTH, &logSize);
			vLog.resize(logSize);

			// エラーが発生している？
			if (logSize > 1)
			{
				glGetShaderInfoLog(newShader, vLog.size(), &length, vLog.data());
				EQ_THROW(String::Sprintf("shader compile error [%s]", vLog.data()).c_str());
			}
			GLint glerr = glGetError();
			if (glerr)
			{
				EQ_THROW(String::Sprintf("glerr 0x%X", glerr).c_str());
			}

			// キャッシュリストに追加
			m_shaderCache.push_front(newShaderCache);

			return newShaderCache;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return nullptr;
	}

	bool Renderer::Impl::SelectProgram(Type type)
	{
		// 同じプログラムなら何もしない
		if (m_currentProgram == type)
		{
			return true;
		}

		// プログラムがキャッシュされているか探し、キャッシュがあればそのまま使う
		for (auto& program : m_programCache)
		{
			if (program->m_type == type)
			{
				glUseProgram(program->m_programID);
				m_currentProgram = type;
				return true;
			}
		}

		// キャッシュに見つからなかったので、新規にプログラムを作成する
		EQ_DURING
		{
			const stShaderDef* pDef = nullptr;

			// 定義を見つける
			for (auto& def : gShaderTbl)
			{
				if (def.m_type == type)
				{
					pDef = &def;
					break;
				}
			}

			if (!pDef)
			{
				// エラー
				EQ_THROW("shader type not found.");
			}

			// バーテックスシェーダを取得
			auto vertexShader = CompileShader(ShaderKind::Vertex, pDef->m_vertexSource);
			if(!vertexShader)
			{
				EQ_THROW("vertex shader compile failed.");
			}

			// フラグメントシェーダを取得
			auto fragmentShader = CompileShader(ShaderKind::Fragment, pDef->m_fragmentSource);
			if (!fragmentShader)
			{
				EQ_THROW("fragmnt shader compile failed.");
			}

			// プログラムキャッシュインスタンス作成
			auto newProgramCache = std::shared_ptr<stProgramCache>(new stProgramCache, [](stProgramCache* p) {
				if (p)
				{
					if (p->m_programID != 0)
					{
						glDeleteShader(p->m_programID);
					}
					delete p;
				}
			});
			if (!newProgramCache)
			{
				EQ_THROW("プログラムキャッシュインスタンスの作成に失敗しました。");
			}

			// プログラムを作成する
			GLuint newProgram = glCreateProgram();
			newProgramCache->m_programID = newProgram;
			newProgramCache->m_vertexCache = vertexShader;
			newProgramCache->m_fragmentCache = fragmentShader;
			newProgramCache->m_type = type;

			// シェーダをアタッチ
			glAttachShader(newProgram, vertexShader->m_id);
			glAttachShader(newProgram, fragmentShader->m_id);

			switch (type)
			{
			case Type::SPRITE:
				glBindAttribLocation(newProgram, 0, "aVertex");
				glBindAttribLocation(newProgram, 1, "a_texCoord");
				glBindAttribLocation(newProgram, 2, "a_color");
				break;
			case Type::LINE:
				glBindAttribLocation(newProgram, 0, "aVertex");
				glBindAttribLocation(newProgram, 1, "a_color");
				break;
			case Type::CIRCLE:
				glBindAttribLocation(newProgram, 0, "aVertex");
				glBindAttribLocation(newProgram, 1, "a_color");
				break;
			}

			GLint linkSuccessful;
			glLinkProgram(newProgram);
			glGetProgramiv(newProgram, GL_LINK_STATUS, &linkSuccessful);
			if (!linkSuccessful)
			{
				EQ_THROW("link program failed.");
			}

			switch (type)
			{
			case Type::SPRITE:
				{
					auto proj = glGetUniformLocation(newProgram, "u_projection");
					auto u_texture = glGetUniformLocation(newProgram, "u_texture");

					glUseProgram(newProgram);

					//-----------------------------------
					// uniform設定
					//-----------------------------------
					glUniformMatrix4fv(proj, 1, GL_FALSE, (GLfloat *)m_projection);
					glUniform1i(u_texture, 0);

					//-----------------------------------
					// VBO作成
					//-----------------------------------
					glGenBuffers(2, m_spriteContext.m_VBO);

					glBindBuffer(GL_ARRAY_BUFFER, m_spriteContext.m_VBO[0]);
					glBufferData(GL_ARRAY_BUFFER, sizeof(m_spriteContext.m_vertex[0]) * m_spriteContext.VBO_SIZE, m_spriteContext.m_vertex, GL_DYNAMIC_DRAW);

					glEnableVertexAttribArray(0);
					glEnableVertexAttribArray(1);
					glEnableVertexAttribArray(2);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_spriteContext.m_VBO[1]);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_spriteContext.m_index[0]) * m_spriteContext.INDEX_VBO_SIZE, m_spriteContext.m_index, GL_STATIC_DRAW);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
					glBindBuffer(GL_ARRAY_BUFFER, 0);

					// GLエラーチェック
				}
				break;

			case Type::LINE:
				{
					auto proj = glGetUniformLocation(newProgram, "u_projection");

					glUseProgram(newProgram);

					//-----------------------------------
					// uniform設定
					//-----------------------------------
					glUniformMatrix4fv(proj, 1, GL_FALSE, (GLfloat *)m_projection);

					//-----------------------------------
					// VBO作成
					//-----------------------------------
					glGenBuffers(2, m_lineContext.m_VBO);

					glBindBuffer(GL_ARRAY_BUFFER, m_lineContext.m_VBO[0]);
					glBufferData(GL_ARRAY_BUFFER, sizeof(m_lineContext.m_vertex[0]) * m_lineContext.VBO_SIZE, m_lineContext.m_vertex, GL_DYNAMIC_DRAW);

					glEnableVertexAttribArray(0);
					glEnableVertexAttribArray(1);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_lineContext.m_VBO[1]);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_lineContext.m_index[0]) * m_lineContext.INDEX_VBO_SIZE, m_lineContext.m_index, GL_STATIC_DRAW);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
					glBindBuffer(GL_ARRAY_BUFFER, 0);

					// GLエラーチェック
				}
				break;

			case Type::CIRCLE:
				{
					auto proj = glGetUniformLocation(newProgram, "u_projection");

					glUseProgram(newProgram);

					//-----------------------------------
					// uniform設定
					//-----------------------------------
					glUniformMatrix4fv(proj, 1, GL_FALSE, (GLfloat *)m_projection);

					//-----------------------------------
					// VBO作成
					//-----------------------------------
					glGenBuffers(2, m_circleContext.m_VBO);

					glBindBuffer(GL_ARRAY_BUFFER, m_circleContext.m_VBO[0]);
					glBufferData(GL_ARRAY_BUFFER, sizeof(m_circleContext.m_vertex[0]) * m_circleContext.VBO_SIZE, m_circleContext.m_vertex, GL_DYNAMIC_DRAW);

					glEnableVertexAttribArray(0);
					glEnableVertexAttribArray(1);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_circleContext.m_VBO[1]);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_circleContext.m_index[0]) * m_circleContext.INDEX_VBO_SIZE, m_circleContext.m_index, GL_STATIC_DRAW);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
					glBindBuffer(GL_ARRAY_BUFFER, 0);

					// GLエラーチェック
				}
				break;
			}

			m_programCache.push_front(newProgramCache);

			m_currentProgram = type;

			return true;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return false;
	}

	void Renderer::Impl::SetProjection(int w, int h)
	{
		m_projection[0][0] = 2.0f / w;
		m_projection[0][1] = 0.0f;
		m_projection[0][2] = 0.0f;
		m_projection[0][3] = 0.0f;

		m_projection[1][0] = 0.0f;
		m_projection[1][1] = -2.0f / h;
		m_projection[1][2] = 0.0f;
		m_projection[1][3] = 0.0f;

		m_projection[2][0] = 0.0f;
		m_projection[2][1] = 0.0f;
		m_projection[2][2] = 0.0f;
		m_projection[2][3] = 0.0f;

		m_projection[3][0] = -1.0f;
		m_projection[3][1] = 1.0f;
		m_projection[3][2] = 0.0f;
		m_projection[3][3] = 1.0f;
	}
}
