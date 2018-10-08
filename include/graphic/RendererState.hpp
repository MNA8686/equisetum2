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

		enum class RenderType : int
		{
			EMPTY = 0,
			PRIMITIVE,
			SPRITE,
			TEXT,
		};

		enum class ShaderType : int
		{
			EMPTY = 0,
			PRIMITIVE,
			SPRITE,
		};

		class PrimitiveType
		{
		public:
			static const int EMPTY  = 0;
			static const int POINT  = (1 << 0);
			static const int LINE   = (1 << 1);
			static const int RECT   = (1 << 2);
			static const int CIRCLE = (1 << 3);
//			static const int TEXT   = (1 << 4);
		};

		typedef struct
		{
			RenderType type;
			int32_t subType;
			BlendMode blend;
			Texture *pTexture;
		}stState;
	}
}

#endif
