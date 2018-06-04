#if !defined(_EQRENDERER_H_)
#define _EQRENDERER_H_

#include "type/String.h"
#include "type/Point.h"
#include "type/Rect.hpp"
#include "graphic/Image.hpp"
#include "graphic/Texture.hpp"
#include "graphic/Sprite.hpp"
//#include "graphic/RenderObject.hpp"
#include <memory>

namespace Equisetum2
{
	/**
	* レンダラクラス
	*/
	class RenderObject;
	class Renderer
	{
	public:

		enum class BlendMode : int
		{
			None,		// ブレンドなし
			Blend,		// アルファブレンド
			Add,		// 加算ブレンド(アルファ有効)
		};
		static const int LayerMax = 64;

		Renderer() = default;
		virtual ~Renderer() = default;

		static std::shared_ptr<Renderer> Create();

		bool AddRenderQueue(RenderObject* pRenderObject);

		bool Copy(const std::shared_ptr<Texture>& tex, const Rect& dst, const Rect& src, int flag, int layer);
		bool CopyWithColor(const std::shared_ptr<Texture>& tex, const Rect& dst, const Rect& src, int flag, int layer, Color color);
		bool Fill(const Rect& dst, int flag, int layer, Color color);

		bool Clear(const Color& color);
		bool Present(bool waitVsync = true /* don't work */);

	private:

		class Impl;
		std::shared_ptr<Impl> m_pImpl;

		std::vector<RenderObject*> m_vRenderObject[LayerMax];
		String m_id;		/// ID

		Renderer(const Renderer&) = delete;					// コピーコンストラクタ封じ
		Renderer& operator= (const Renderer&) = delete;		// コピーコンストラクタ封じ
		Renderer& operator= (const Renderer&&) = delete;	// ムーブセマンティクスコンストラクタ封じ

		void SortRenderQueue();
	};
}

#endif
