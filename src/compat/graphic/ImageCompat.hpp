#if !defined(_EQIMAGECOMPAT_H_)
#define _EQIMAGECOMPAT_H_

#include "system/Singleton.h"
#include "graphic/Image.hpp"

namespace Equisetum2
{
	class ImageCompat final
	{
	public:
		friend Singleton<ImageCompat>;	// シングルトンからインスタンスを作成してもらえるようにする

		bool InitFromStream(std::shared_ptr<IStream>& stream);
		bool SaveToStream(std::shared_ptr<IStream> stream);
		bool Resize(uint32_t width, uint32_t height);
		uint32_t Width();
		uint32_t Height();
		uint32_t Pitch();
		Color* Data();

	private:

		class Impl;
		std::shared_ptr<Impl> m_Impl;
		ImageCompat();	// インスタンス作成封じ
	};
}

#endif
