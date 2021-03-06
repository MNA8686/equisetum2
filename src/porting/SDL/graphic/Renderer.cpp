﻿#include "system/Exception.hpp"
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

	static const char *vertexShaderSpriteSrc = R"(#version 120
		#define IN         attribute
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
			v_color = ((a_color - 128) * inv255f) * 2;
			// UV座標
			v_texCoord = a_texCoord;
			// 表示座標
			gl_Position = u_projection * vec4(aVertex, 0, 1.0);
		}
	)";

	static const char *fragmentShaderSpriteSrc = R"(#version 120
		#define IN         attribute
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

	static const char *vertexShaderPrimitiveSrc = R"(#version 120
		#define IN         attribute
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

	static const char *fragmentShaderPrimitiveSrc = R"(#version 120
		#define IN         attribute
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
		ShaderType m_type;		// シェーダ用途タイプ
		const char* m_vertexSource;			// バーテックスシェーダのソース
		const char* m_fragmentSource;		// フラグメントシェーダのソース
	}stShaderDef;

	// シェーダ定義テーブル
	static const stShaderDef gShaderTbl[] =
	{
		{
			ShaderType::SPRITE,
			vertexShaderSpriteSrc,
			fragmentShaderSpriteSrc
		},
		{
			ShaderType::PRIMITIVE,
			vertexShaderPrimitiveSrc,
			fragmentShaderPrimitiveSrc
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
				inst->m_vRenderObject[i].resize(1024);
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

			// フレームバッファを取得
			GLint framebuffer = -1;
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebuffer);
			inst->m_pImpl->m_framebuffer = static_cast<GLuint>(framebuffer);

			// ビューポートを設定
			int w, h;
			SDL_GetWindowSize(pWindow.get(), &w, &h);
			inst->SetViewport({ 0, 0, w, h });

			// 画面をクリア
			glClearColor((GLfloat)0, (GLfloat)0, (GLfloat)0, (GLfloat)0);
			glClear(GL_COLOR_BUFFER_BIT);

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

	bool Renderer::SetViewport(const Rect& rect)
	{
		if (rect.width == 0 || rect.height == 0)
		{
			return false;
		}

		if (m_renderTarget)
		{
			// ビューポートを設定
			glViewport(rect.x, rect.y, rect.width, rect.height);
		}
		else
		{
			int w, h;
			SDL_GetWindowSize(m_pImpl->m_attachedWindow.get(), &w, &h);

			// ビューポートを設定
			glViewport(rect.x, h - rect.y - rect.height, rect.width, rect.height);
		}

		// ウィンドウのビューポートを保存
		if (!m_renderTarget)
		{
			m_viewport = rect;
		}

		// プロジェクションマトリックス更新
		{
			auto& proj = m_pImpl->m_projection;

			proj[0][0] = 2.0f / rect.width;
			proj[0][1] = 0.0f;
			proj[0][2] = 0.0f;
			proj[0][3] = 0.0f;

			proj[1][0] = 0.0f;
			proj[1][1] = (m_renderTarget ? 2.0f : -2.0f) / rect.height;
			proj[1][2] = 0.0f;
			proj[1][3] = 0.0f;

			proj[2][0] = 0.0f;
			proj[2][1] = 0.0f;
			proj[2][2] = 0.0f;
			proj[2][3] = 0.0f;

			proj[3][0] = -1.0f;
			proj[3][1] = m_renderTarget ? -1.0f : 1.0f;
			proj[3][2] = 0.0f;
			proj[3][3] = 1.0f;
		}

		for (auto& program : m_pImpl->m_programCache)
		{
			// 現在存在しているプログラムに更新をかける
			program->m_projectionDirty = true;
		}

		// 現在設定されているプログラムにプロジェクションをセット
		if (m_pImpl->m_currentProgramType != ShaderType::EMPTY)
		{
			SetOrthographicProjection();
		}

		return true;
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
			auto& obj = m_vRenderObject[layer];
			auto& index = m_renderObjectIndex[layer];

			// サイズが足りない場合は拡張する
			if (static_cast<size_t>(index + 1) >= obj.size())
			{
				obj.resize(obj.size() * 2);
			}


			obj[index] = pRenderObject;
			index++;
			return true;
		}

		return false;
	}

	void Renderer::SortRenderQueue()
	{
		int layer = 0;

		for (auto& objectsInLayer : m_vRenderObject)
		{
			auto& index = m_renderObjectIndex[layer];
			std::sort(std::begin(objectsInLayer), std::begin(objectsInLayer) + index, [](RenderObject* a, RenderObject* b)->bool {
				return  a->GetOrderInLayer() < b->GetOrderInLayer();
			});

			layer++;
		}
	}

	static int gDrawCallCount = 0;

	bool Renderer::Render()
	{
		auto& pWindow = Singleton<WindowCompat>::GetInstance()->m_Impl->GetWindowPtr();
		auto& spriteContext = m_pImpl->m_spriteContext;
		auto& primitiveContext = m_pImpl->m_primitiveContext;

		gDrawCallCount = 0;

		SDL_GL_MakeCurrent(pWindow.get(), *(m_pImpl->m_GLContext));

		// ソートする
		SortRenderQueue();

		// カレントステートをリセット
		m_currentStates.type = RenderType::EMPTY;
		m_currentStates.blend = BlendMode::None;
		m_currentStates.pTexture = nullptr;

		spriteContext.m_filledVertexNum = 0;
		spriteContext.m_filledIndexNum = 0;
		primitiveContext.pPrimitiveRender = nullptr;

		int layer = 0;
		for (auto& objectsInLayer : m_vRenderObject)
		{
			int32_t objectMax = m_renderObjectIndex[layer++];
			for (int i = 0; i < objectMax; i++)
			{
				auto& renderObject = objectsInLayer[i];

				// 表示内容を算出する
				if (!renderObject->Calculation())
				{
					continue;
				}

				if (renderObject->GetType() == RenderType::SPRITE)
				{
					auto spriteRenderer = static_cast<SpriteRenderer*>(renderObject);

					// このスプライトの頂点数
					size_t vertexCount = spriteRenderer->m_pImpl->GetVertexCount();
					RenderState::BlendMode blendMode = spriteRenderer->m_blend;
					Texture* pTexture = spriteRenderer->m_sprite->GetTexture().get();

					// 頂点配列が全て埋まっている？またはステートが変化した？
					if (m_currentStates.type != RenderType::SPRITE ||
						*(m_currentStates.pTexture->m_pImpl->GetTexID()) != *(pTexture->m_pImpl->GetTexID()) ||
						m_currentStates.blend != blendMode ||
						spriteContext.m_filledVertexNum + vertexCount >= spriteContext.VBO_SIZE)
					{
						// 描画を行う
						DrawCall();

						// ステートを更新
						m_currentStates.type = RenderType::SPRITE;
						m_currentStates.subType = 0;
						m_currentStates.blend = blendMode;
						m_currentStates.pTexture = pTexture;
					}

					// 頂点配列を埋める
					const stVertexSprite* vertex = spriteRenderer->m_pImpl->GetVertex();		// このスプライトの頂点バッファ
					memcpy(&spriteContext.m_vertex[spriteContext.m_filledVertexNum], vertex, sizeof(stVertexSprite) * vertexCount);

					// インデックス配列を埋める
					const GLushort* index = spriteRenderer->m_pImpl->GetIndex();		// このスプライトのインデックスバッファ
					size_t indexCount = spriteRenderer->m_pImpl->GetIndexCount();		// このスプライトのインデックス数
					for (decltype(indexCount) i = 0; i < indexCount; i++)
					{
						// 頂点の番号を変換しながらコピーする
						spriteContext.m_index[spriteContext.m_filledIndexNum + i] = static_cast<GLushort>(spriteContext.m_filledVertexNum + index[i]);
					}

					spriteContext.m_filledVertexNum += vertexCount;
					spriteContext.m_filledIndexNum += indexCount;
				}
				else if (renderObject->GetType() == RenderType::PRIMITIVE)
				{
					auto primitiveRenderer = static_cast<PrimitiveRenderer*>(renderObject);

					// このオブジェクトの頂点数
					size_t vertexCount = primitiveRenderer->m_pImpl->GetVertexCount();
					RenderState::BlendMode blendMode = primitiveRenderer->m_pImpl->GetBlendMode();

					if (vertexCount > 0)
					{
						// 描画を行う
						DrawCall();

						// ステートを更新
						m_currentStates.type = renderObject->GetType();
						m_currentStates.subType = renderObject->GetSubType();
						m_currentStates.blend = blendMode;

						// Primitiveの場合はレンダーオブジェクトの構造体を直接参照すれば事足りる
						primitiveContext.pPrimitiveRender = primitiveRenderer;

						// 描画を行う
						DrawCall();

						m_currentStates.type = RenderType::EMPTY;
						m_currentStates.subType = 0;
					}
				}
				else if (renderObject->GetType() == RenderType::TEXT)
				{
					// 実質スプライトの描画と同じなので、コンテキストはスプライトのものを使用する

					auto textRenderer = static_cast<TextRenderer*>(renderObject);

					// このオブジェクトの頂点数
					size_t vertexCount = textRenderer->m_pImpl->GetVertexCount();
					size_t indexCount = textRenderer->m_pImpl->GetIndexCount();
					RenderState::BlendMode blendMode = textRenderer->m_blend;
					Texture* pTexture = textRenderer->m_bitmapFont ? textRenderer->m_bitmapFont->GetSprite()->GetTexture().get() : nullptr;

					if (vertexCount > 0)
					{
						// 描画を行う
						DrawCall();

						// ステートを更新
						m_currentStates.type = RenderType::TEXT;
						m_currentStates.subType = 0;
						m_currentStates.blend = blendMode;
						m_currentStates.pTexture = pTexture;

						// 頂点配列を埋める
						const stVertexSprite* vertex = textRenderer->m_pImpl->GetVertex();		// このスプライトの頂点バッファ
						memcpy(spriteContext.m_vertex, vertex, sizeof(stVertexSprite) * vertexCount);

						// インデックス配列を埋める
						const GLushort* index = textRenderer->m_pImpl->GetIndex();		// このスプライトのインデックスバッファ
						memcpy(spriteContext.m_index, index, sizeof(GLushort) * indexCount);

						spriteContext.m_filledVertexNum += vertexCount;
						spriteContext.m_filledIndexNum += indexCount;

						// 描画を行う
						DrawCall();

						m_currentStates.type = RenderType::EMPTY;
						m_currentStates.subType = 0;
					}
				}
			}
		}

		// バッファに残っているものを描画する
		DrawCall();

		// レンダーキューをクリアする
		ClearRenderQueue();

		//Logger::OutputDebug("draw call %d", gDrawCallCount);

		return true;
	}

	bool Renderer::DrawCall()
	{
		if (m_currentStates.type == RenderType::SPRITE ||
			m_currentStates.type == RenderType::TEXT)
		{
			auto& ctx = m_pImpl->m_spriteContext;

			if (SelectProgram(ShaderType::SPRITE))
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
		else if (m_currentStates.type == RenderType::PRIMITIVE)
		{
			auto& ctx = m_pImpl->m_primitiveContext;

			if (SelectProgram(ShaderType::PRIMITIVE))
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

				const auto& inst = ctx.pPrimitiveRender->m_pImpl;
				const stVertexPrimitive* pVertex = inst->GetVertex();
				size_t filledVertexNum = inst->GetVertexCount();
				bool solid = inst->GetSolid();

				glBindBuffer(GL_ARRAY_BUFFER, ctx.m_VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(stVertexPrimitive) * filledVertexNum, pVertex, GL_DYNAMIC_DRAW);


				if (m_currentStates.subType == PrimitiveType::LINE)
				{
					glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(stVertexPrimitive), 0);
					glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(stVertexPrimitive), (const void*)(2 * sizeof(GLfloat)));

					glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(filledVertexNum));
				}
				else if (m_currentStates.subType == PrimitiveType::RECT)
				{
					glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(stVertexPrimitive), 0);
					glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(stVertexPrimitive), (const void*)(2 * sizeof(GLfloat)));

					glDrawArrays(solid ? GL_TRIANGLE_STRIP : GL_LINE_LOOP, 0, static_cast<GLsizei>(filledVertexNum));
				}
				else if (m_currentStates.subType == PrimitiveType::CIRCLE)
				{
					glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(stVertexPrimitive), 0);
					glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(stVertexPrimitive), (const void*)(2 * sizeof(GLfloat)));

					glDrawArrays(solid ? GL_TRIANGLE_FAN : GL_LINE_STRIP, 0, static_cast<GLsizei>(filledVertexNum));
				}

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

				gDrawCallCount++;
			}
		}

		return true;
	}

	void Renderer::ClearRenderQueue()
	{
		// レンダーキューをクリアする
		for (auto& index : m_renderObjectIndex)
		{
			index = 0;
		}
	}

	std::shared_ptr<RenderObject> Renderer::_CreateRenderObject(RenderType type, int32_t subType)
	{
		std::shared_ptr<RenderObject> obj;

		switch (type)
		{
		case RenderType::SPRITE:
			obj = SpriteRenderer::Create();
			break;

		case RenderType::TEXT:
			obj = TextRenderer::Create();
			break;

		case RenderType::PRIMITIVE:

			switch(subType)
			{
			case PrimitiveType::LINE:
				obj = LineRenderer::Create();
				break;
			case PrimitiveType::RECT:
				obj = RectRenderer::Create();
				break;
			case PrimitiveType::CIRCLE:
				obj = CircleRenderer::Create();
				break;
			}
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

	bool Renderer::SelectProgram(ShaderType type)
	{
		// 同じプログラムなら何もしない
		if (m_pImpl->m_currentProgramType == type)
		{
			return true;
		}

		// プログラムがキャッシュされているか探し、キャッシュがあればそのまま使う
		for (auto& program : m_pImpl->m_programCache)
		{
			if (program->m_type == type)
			{
				glUseProgram(program->m_programID);
				m_pImpl->m_currentProgramType = type;

				// プロジェクションが更新されているかもしれないのでプロジェクションの設定を実行
				SetOrthographicProjection();
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
			auto vertexShader = m_pImpl->CompileShader(ShaderKind::Vertex, pDef->m_vertexSource);
			if(!vertexShader)
			{
				EQ_THROW("vertex shader compile failed.");
			}

			// フラグメントシェーダを取得
			auto fragmentShader = m_pImpl->CompileShader(ShaderKind::Fragment, pDef->m_fragmentSource);
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
			case ShaderType::SPRITE:
				glBindAttribLocation(newProgram, 0, "aVertex");
				glBindAttribLocation(newProgram, 1, "a_texCoord");
				glBindAttribLocation(newProgram, 2, "a_color");
				break;
			case ShaderType::PRIMITIVE:
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
			case ShaderType::SPRITE:
				{
					auto u_texture = glGetUniformLocation(newProgram, "u_texture");

					glUseProgram(newProgram);

					//-----------------------------------
					// uniform設定
					//-----------------------------------
					glUniform1i(u_texture, 0);

					//-----------------------------------
					// VBO作成
					//-----------------------------------
					glGenBuffers(2, m_pImpl->m_spriteContext.m_VBO);

					glBindBuffer(GL_ARRAY_BUFFER, m_pImpl->m_spriteContext.m_VBO[0]);
					glBufferData(GL_ARRAY_BUFFER, sizeof(m_pImpl->m_spriteContext.m_vertex[0]) * m_pImpl->m_spriteContext.VBO_SIZE, m_pImpl->m_spriteContext.m_vertex, GL_DYNAMIC_DRAW);

					glEnableVertexAttribArray(0);
					glEnableVertexAttribArray(1);
					glEnableVertexAttribArray(2);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pImpl->m_spriteContext.m_VBO[1]);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_pImpl->m_spriteContext.m_index[0]) * m_pImpl->m_spriteContext.INDEX_VBO_SIZE, m_pImpl->m_spriteContext.m_index, GL_STATIC_DRAW);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
					glBindBuffer(GL_ARRAY_BUFFER, 0);

					// GLエラーチェック
				}
				break;

			case ShaderType::PRIMITIVE:
				{
					auto& ctx = m_pImpl->m_primitiveContext;
					const int len = 8192;
					std::vector<stVertexPrimitive> vVertex(len);
					std::vector<GLushort> vIndex(len);

					glUseProgram(newProgram);

					//-----------------------------------
					// uniform設定
					//-----------------------------------

					//-----------------------------------
					// VBO作成
					//-----------------------------------
					glGenBuffers(1, &ctx.m_VBO);

					glBindBuffer(GL_ARRAY_BUFFER, ctx.m_VBO);
					glBufferData(GL_ARRAY_BUFFER, sizeof(stVertexPrimitive) * len, vVertex.data(), GL_DYNAMIC_DRAW);

					glEnableVertexAttribArray(0);
					glEnableVertexAttribArray(1);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

					// GLエラーチェック
				}
				break;
			}

			// プログラムキャッシュに追加
			m_pImpl->m_programCache.push_front(newProgramCache);

			// 現在設定されているプログラムの種類を保持
			m_pImpl->m_currentProgramType = type;

			// プロジェクションを設定
			SetOrthographicProjection();

			return true;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return false;
	}

	void Renderer::SetOrthographicProjection()
	{
		for (auto& program : m_pImpl->m_programCache)
		{
			if (program->m_type == m_pImpl->m_currentProgramType)
			{
				if (program->m_projectionDirty)
				{
					GLint projID = glGetUniformLocation(program->m_programID, "u_projection");

					glUniformMatrix4fv(projID, 1, GL_FALSE, reinterpret_cast<GLfloat*>(m_pImpl->m_projection));

					program->m_projectionDirty = false;
				}
				break;
			}
		}
	}

	bool Renderer::SetRenderTarget(std::shared_ptr<Texture> texture)
	{
		bool ret = true;

		if (m_renderTarget != texture)
		{
			// レンダリング先がテクスチャになるときにウィンドウの設定を保存
			if (texture && !m_renderTarget)
			{
				m_viewportBak = m_viewport;
				m_clipRectBak = m_clipRect;
				m_clippingEnabledBak = m_clippingEnabled;
			}

			if (texture)
			{
				// テクスチャのサイズを設定
				m_viewport = { 0,
								0,
								static_cast<int32_t>(texture->Width()),
								static_cast<int32_t>(texture->Height()) };
				m_clipRect = {};
				m_clippingEnabled = false;
			}
			else
			{
				// ウィンドウの設定を復元
				m_viewport = m_viewportBak;
				m_clipRect = m_clipRectBak;
				m_clippingEnabled = m_clippingEnabledBak;
			}

			glBindFramebuffer(GL_FRAMEBUFFER, texture ? *(texture->m_pImpl->GetFBO()) : m_pImpl->m_framebuffer);
			m_renderTarget = texture;
			ret = SetViewport(m_viewport);
			if (ret)
			{
				SetClipRect(m_clipRect);
			}

			// レンダーキュークリア
			ClearRenderQueue();
		}

		return ret;
	}

	void Renderer::SetClipRect(const Rect& rect)
	{
		if (rect.width == 0 ||
			rect.height == 0)
		{
			m_clippingEnabled = false;
			glDisable(GL_SCISSOR_TEST);
		}
		else
		{
			m_clippingEnabled = true;
			m_clipRect = rect;

			glEnable(GL_SCISSOR_TEST);

			if (m_renderTarget)
			{
				glScissor(m_viewport.x + rect.x,
					m_viewport.y + rect.y,
					rect.width,
					rect.height);
			}
			else
			{
				int w, h;
				SDL_GetWindowSize(m_pImpl->m_attachedWindow.get(), &w, &h);

				glScissor(m_viewport.x + rect.x,
					h - m_viewport.y - rect.y - rect.height,
					rect.width,
					rect.height);
			}
		}
	}

	void CurrentRenderer::Set(const std::shared_ptr<Renderer>& renderer)
	{
		m_renderer = renderer;
	}

	std::shared_ptr<Renderer> CurrentRenderer::Get()
	{
		return m_renderer.lock();
	}
}

