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

			auto rawInst = new (std::nothrow) Image;
			if (!rawInst)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			auto inst_ = std::shared_ptr<Image>(rawInst);
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

			auto inst_ = std::shared_ptr<Image>(new Image);
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

	bool Image::SaveToStream(std::shared_ptr<IStream> stream) const
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

	bool Image::CopyTo(std::shared_ptr<Image> dstImage, const Optional<Rect> src, const Optional<Rect> dst) const
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

			if (src)
			{
				auto& rect = *src;
				if (rect.x < 0 ||
					rect.width <= 0 ||
					rect.x + rect.width > static_cast<int32_t>(Width()) ||
					rect.y < 0 ||
					rect.height <= 0 ||
					rect.y + rect.height > static_cast<int32_t>(Height()))
				{
					EQ_THROW(u8"転送元範囲の指定が不正です。");
				}
			}

			if (dst)
			{
				auto& rect = *dst;
				if (rect.x < 0 ||
					rect.width <= 0 ||
					rect.x + rect.width > static_cast<int32_t>(dstImage->Width()) ||
					rect.y < 0 ||
					rect.height <= 0 ||
					rect.y + rect.height > static_cast<int32_t>(dstImage->Height()))
				{
					EQ_THROW(u8"転送先範囲の指定が不正です。");
				}
			}

			if (!m_pImpl->CopyTo(dstImage, src, dst))
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

	bool Image::MoveFrom(std::shared_ptr<Image>&& src)
	{
		m_pImpl = src->m_pImpl;
		m_id = src->m_id;
		src->m_id.clear();

		return true;
	}

	uint32_t Image::Width() const
	{
		return m_pImpl->Width();
	}

	uint32_t Image::Height() const
	{
		return m_pImpl->Height();
	}

	uint32_t Image::Pitch() const
	{
		return m_pImpl->Pitch();
	}

	Color* Image::Data()
	{
		return m_pImpl->Data();
	}

	const Color* Image::Data() const
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

