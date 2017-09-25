#include "graphic/Image.hpp"
#include "system/Logger.h"
#include "system/Exception.hpp"
#include "graphic/ImageCompat.hpp"

namespace Equisetum2
{
	std::shared_ptr<Image> Image::CreateFromStream(std::shared_ptr<IStream> stream)
	{
		std::shared_ptr<Image> inst;

		EQ_DURING
		{
			if (!stream)
			{
				EQ_THROW(u8"有効なストリームではありません。");
			}

			if (!stream->CanRead())
			{
				EQ_THROW(u8"リード属性が必要です。");
			}

			class ImageDummy : public Image
			{
			public:
				explicit ImageDummy() {};
			};

			auto inst_ = std::make_shared<ImageDummy>();

			if (!inst_)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			if (!inst_->InitFromStream(stream))
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			inst = inst_;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return inst;
	}

	bool Image::InitFromStream(std::shared_ptr<IStream>& stream)
	{
		return Singleton<ImageCompat>::GetInstance()->InitFromStream(stream);
	}

	bool Image::SaveToStream(std::shared_ptr<IStream> stream)
	{
		auto ret = false;

		EQ_DURING
		{
			if (!stream)
			{
				EQ_THROW(u8"有効なストリームではありません。");
			}

			if (!stream->CanWrite())
			{
				EQ_THROW(u8"ライト属性が必要です。");
			}

			if (!stream->CanSeek())
			{
				EQ_THROW(u8"シーク属性が必要です。");
			}

			if (!Singleton<ImageCompat>::GetInstance()->SaveToStream(stream))
			{
				EQ_THROW(u8"イメージの出力に失敗しました。");
			}

			ret = true;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return ret;
	}

	bool Image::Resize(uint32_t width, uint32_t height)
	{
		return Singleton<ImageCompat>::GetInstance()->Resize(width, height);
	}

	uint32_t Image::Width()
	{
		return Singleton<ImageCompat>::GetInstance()->Width();
	}

	uint32_t Image::Height()
	{
		return Singleton<ImageCompat>::GetInstance()->Height();
	}

	uint32_t Image::Pitch()
	{
		return Singleton<ImageCompat>::GetInstance()->Pitch();
	}

	Color* Image::Data()
	{
		return Singleton<ImageCompat>::GetInstance()->Data();
	}
}

