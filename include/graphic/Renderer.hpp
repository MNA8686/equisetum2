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
		bool Render();

		std::shared_ptr<RenderObject> CreateRenderObject(Type type, int32_t subType=0);

		bool Clear(const Color& color);
		bool Present(bool waitVsync = true /* don't work */);

		class Impl;
		std::shared_ptr<Impl> m_pImpl;

	private:

		std::vector<RenderObject*> m_vRenderObject[LayerMax];		// レンダーキュー
		//String m_id;		/// ID

		stState m_currentStates = {};
		bool DrawCall();

		Renderer(const Renderer&) = delete;					// コピーコンストラクタ封じ
		Renderer& operator= (const Renderer&) = delete;		// コピーコンストラクタ封じ
		Renderer& operator= (const Renderer&&) = delete;	// ムーブセマンティクスコンストラクタ封じ

		void SortRenderQueue();
	};
}

#endif
