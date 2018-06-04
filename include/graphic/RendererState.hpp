#if !defined(_EQRENDERSTATE_H_)
#define _EQRENDERSTATE_H_

namespace Equisetum2
{
	namespace RenderState
	{
		enum class BlendMode : int
		{
			None,		// ブレンドなし
			Blend,		// アルファブレンド
			Add,		// 加算ブレンド(アルファ有効)
		};

		enum class Type : int
		{
			EMPTY = 0,
			POINT,
			LINE,
			RECT,
			CIRCLE,
			SPRITE,
		};

		typedef struct
		{
			Type type;
			BlendMode blend;
			Texture *pTexture;
		}stState;
	}
}

#endif
