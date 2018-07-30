#if !defined(_EQRENDERER_H_)
#define _EQRENDERER_H_

#include "type/String.h"
#include "type/Point.h"
#include "type/Rect.hpp"
#include "graphic/Image.hpp"
#include "graphic/Texture.hpp"
#include "graphic/Sprite.hpp"
#include "graphic/RenderObject.hpp"
#include <memory>

namespace Equisetum2
{
	/**
	* レンダラクラス
	*/
	class RenderObject;
	class Renderer : public std::enable_shared_from_this<Renderer>
	{
	public:

		static const int LayerMax = 64;

		Renderer() = default;
		virtual ~Renderer() = default;

		static std::shared_ptr<Renderer> Create();

		bool AddRenderQueue(RenderObject* pRenderObject);
		void ClearRenderQueue();
		bool Render();
		bool SetRenderTarget(std::shared_ptr<Texture> texture);
		bool SetViewport(const Rect& rect);
		void SetClipRect(const Rect& rect = {});

		std::shared_ptr<RenderObject> CreateRenderObject(Type type, int32_t subType=0);

		bool Clear(const Color& color);
		bool Present(bool waitVsync = true /* don't work */);

		class Impl;
		std::shared_ptr<Impl> m_pImpl;

	private:

		std::vector<RenderObject*> m_vRenderObject[LayerMax];		// レンダーキュー
		int32_t m_renderObjectIndex[LayerMax]{0};					/// レンダーキューのサイズ
		//String m_id;		/// ID
		std::shared_ptr<Texture> m_renderTarget;					/// レンダーターゲット nullptrの場合、ウィンドウに描画。 <br>
																	///                    nullptr以外の場合、設定されたテクスチャに描画。
		Rect m_viewport;		/// 現在のビューポート
		Rect m_viewportBak;		/// ウィンドウのビューポート退避用
		Rect m_clipRect;		/// 現在のクリップ領域
		Rect m_clipRectBak;		/// ウィンドウのクリップ領域退避用
		bool m_clippingEnabled = false;		/// 現在のクリッピング設定
		bool m_clippingEnabledBak = false;	/// ウィンドウのクリッピング設定退避用

		stState m_currentStates = {};
		bool DrawCall();

		Renderer(const Renderer&) = delete;					// コピーコンストラクタ封じ
		Renderer& operator= (const Renderer&) = delete;		// コピーコンストラクタ封じ
		Renderer& operator= (const Renderer&&) = delete;	// ムーブセマンティクスコンストラクタ封じ

		void SortRenderQueue();
		void SetOrthographicProjection();
		bool SelectProgram(Type type);
	};
}

#endif
