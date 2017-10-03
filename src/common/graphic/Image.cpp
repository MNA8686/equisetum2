#include "graphic/Image.hpp"
#include "system/Logger.h"
#include "system/Exception.hpp"

#include "graphic/ImageCompatImpl.inl"

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
				explicit ImageDummy(){};
			};

			auto inst_ = std::make_shared<ImageDummy>();
			if (!inst_)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			inst_->m_pImpl = std::make_shared<Image::Impl>();
			if (!inst_->m_pImpl)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			if (!inst_->m_pImpl->InitFromStream(stream))
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

	std::shared_ptr<Image> Image::CreateBlank(uint32_t width, uint32_t height)
	{
		std::shared_ptr<Image> inst;

		EQ_DURING
		{
			if (width == 0 || width >= 65536)
			{
				EQ_THROW(u8"横幅が不正です。");
			}

			if (height == 0 || height >= 65536)
			{
				EQ_THROW(u8"縦幅が不正です。");
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

			inst_->m_pImpl = std::make_shared<Image::Impl>();
			if (!inst_->m_pImpl)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			if (!inst_->m_pImpl->InitBlank(width, height))
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

			if (!m_pImpl->SaveToStream(stream))
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
		auto ret = false;

		EQ_DURING
		{
			if (width == 0 || width >= 65536)
			{
				EQ_THROW(u8"横幅が不正です。");
			}

			if (height == 0 || height >= 65536)
			{
				EQ_THROW(u8"縦幅が不正です。");
			}

			if (m_pImpl->Width() != width ||
				m_pImpl->Height() != height)
			{
				if (!m_pImpl->Resize(width, height))
				{
					EQ_THROW(u8"イメージのリサイズに失敗しました。");
				}
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

	bool Image::CopyTo(std::shared_ptr<Image> dstImage)
	{
		auto ret = false;

		EQ_DURING
		{
			if (!dstImage)
			{
				EQ_THROW(u8"コピー先イメージのポインタが不正です。");
			}

			if (this == dstImage.get())
			{
				EQ_THROW(u8"コピー元とコピー先のイメージが同じです。");
			}

			if (dstImage->Width() == 0 || dstImage->Width() >= 65536)
			{
				EQ_THROW(u8"コピー先イメージの横幅が不正です。");
			}

			if (dstImage->Height() == 0 || dstImage->Height() >= 65536)
			{
				EQ_THROW(u8"コピー先イメージの縦幅が不正です。");
			}

			if (!m_pImpl->CopyTo(dstImage))
			{
				EQ_THROW(u8"イメージのコピーに失敗しました。");
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

	bool Image::Replace(std::shared_ptr<Image> src)
	{
		return m_pImpl->Replace(src);
	}

	uint32_t Image::Width()
	{
		return m_pImpl->Width();
	}

	uint32_t Image::Height()
	{
		return m_pImpl->Height();
	}

	uint32_t Image::Pitch()
	{
		return m_pImpl->Pitch();
	}

	Color* Image::Data()
	{
		return m_pImpl->Data();
	}

	void Image::SetIdentify(const String& id)
	{
		m_id = id;
	}

	String Image::Identify() const
	{
		return m_id;
	}
}

