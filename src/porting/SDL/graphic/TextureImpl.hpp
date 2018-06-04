#if !defined(_EQTEXTUREIMPL_H_)
#define _EQTEXTUREIMPL_H_

#include "graphic/Texture.hpp"
#include "glew.h"

namespace Equisetum2
{
	class Texture::Impl final
	{
	private:
		friend class Texture;
		uint32_t m_width = 0;		/// テクスチャの横幅
		uint32_t m_height = 0;		/// テクスチャの縦幅
		std::shared_ptr<GLuint> m_texID;		/// テクスチャID

	public:
		std::shared_ptr<GLuint>& GetTexID()
		{
			return m_texID;
		}
	};
}

#endif
