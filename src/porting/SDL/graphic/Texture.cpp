#include "system/Exception.hpp"
#include "system/Logger.h"
#include "graphic/Texture.hpp"
#include "graphic/TextureImpl.hpp"

namespace Equisetum2
{
	std::shared_ptr<Texture> Texture::CreateFromImage(const std::shared_ptr<Image> image)
	{
		EQ_DURING
		{
			if (!image)
			{
				EQ_THROW(u8"イメージが設定されていません。");
			}

			if (image->Width() == 0 ||
				image->Height() == 0)
			{
				EQ_THROW(u8"イメージのサイズが不正です。");
			}

			::glActiveTexture(GL_TEXTURE0);
			//glEnable(GL_TEXTURE_2D);

			// テクスチャのデリーター作成
			auto spTexID = std::shared_ptr<GLuint>(new GLuint, [](GLuint* pID) {
				::glDeleteTextures(1, pID);
				delete pID;
			});

			{
				GLint max = 0;
				::glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
				max = 0;
			}

			::glGenTextures(1, spTexID.get());
			::glBindTexture(GL_TEXTURE_2D, *spTexID);

			::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			::glTexImage2D(
				GL_TEXTURE_2D,
				0,					// mipmap
				GL_RGBA,
				image->Width(),		// width
				image->Height(),	// height
				0,					// border
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				image->Data()
			);

			// インスタンス作成
			auto inst = std::shared_ptr<Texture>(new Texture);
			if (!inst)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			// インスタンス初期化
			inst->m_pImpl = std::make_shared<Texture::Impl>();
			if (!inst->m_pImpl)
			{
				EQ_THROW(u8"インスタンスの初期化に失敗しました。");
			}

			inst->m_pImpl->m_width = image->Width();
			inst->m_pImpl->m_height = image->Height();
			inst->m_pImpl->m_texID = spTexID;

			return inst;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return nullptr;
	}

	std::shared_ptr<Texture> Texture::CreateBlank(uint32_t width, uint32_t height, int32_t flag)
	{
		EQ_DURING
		{
			::glActiveTexture(GL_TEXTURE0);

			// テクスチャのデリーター作成
			auto spTexID = std::shared_ptr<GLuint>(new GLuint,
				[](GLuint* pID) {
				::glDeleteTextures(1, pID);
				delete pID;
			});

			::glGenTextures(1, spTexID.get());
			::glBindTexture(GL_TEXTURE_2D, *spTexID);

			::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			::glTexImage2D(
				GL_TEXTURE_2D,
				0,					// mipmap
				GL_RGBA,
				width,				// width
				height,				// height
				0,					// border
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				nullptr
			);

			// インスタンス作成
			auto inst = std::shared_ptr<Texture>(new Texture);
			if (!inst)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			// インスタンス初期化
			inst->m_pImpl = std::make_shared<Texture::Impl>();
			if (!inst->m_pImpl)
			{
				EQ_THROW(u8"インスタンスの初期化に失敗しました。");
			}

			inst->m_flag = flag;
			inst->m_pImpl->m_width = width;
			inst->m_pImpl->m_height = height;
			inst->m_pImpl->m_texID = spTexID;

			if (inst->m_flag & AccessFlag::RenderTarget)
			{
				auto spFBO = std::shared_ptr<GLuint>(new GLuint,
					[](GLuint* pID) {
					::glDeleteFramebuffers(1, pID);
					delete pID;
				});

				glGenFramebuffers(1, spFBO.get());
				inst->m_pImpl->m_FBO = spFBO;
			}

			return inst;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return nullptr;
	}

	bool Texture::Update(const Point& pos, const std::shared_ptr<Image> image)
	{
		EQ_DURING
		{
			if (!image)
			{
				EQ_THROW(u8"イメージが設定されていません。");
			}

			if (image->Width() == 0 ||
				image->Height() == 0)
			{
				EQ_THROW(u8"イメージのサイズが不正です。");
			}

			::glActiveTexture(GL_TEXTURE0);

			::glBindTexture(GL_TEXTURE_2D, *(m_pImpl->m_texID));
			::glTexSubImage2D(GL_TEXTURE_2D, 0, pos.x, pos.y, image->Width(), image->Height(),
				GL_RGBA, GL_UNSIGNED_BYTE, image->Data());

			return true;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return false;
	}

	uint32_t Texture::Width() const
	{
		return m_pImpl->m_width;
	}

	uint32_t Texture::Height() const
	{
		return m_pImpl->m_height;
	}

	bool Texture::MoveFrom(std::shared_ptr<Texture>&& src)
	{
		m_pImpl = std::move(src->m_pImpl);
		m_id = src->m_id;
		return true;
	}

	void Texture::SetIdentify(const String& id)
	{
		m_id = id;
	}

	String Texture::Identify() const
	{
		return m_id;
	}
}
