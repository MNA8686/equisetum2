#include "system/Exception.hpp"
#include "system/Logger.h"
#include "util/AssetManager.hpp"
#include "util/SharedPool.hpp"
#include "stream/FileStream.h"
#include "fs/Path.hpp"
#include "fs/File.hpp"

#include "cereal/external/rapidjson/document.h"
#include "cereal/external/rapidjson/reader.h"
#include "cereal/external/rapidjson/error/error.h"
#include "cereal/external/rapidjson/error/en.h"

namespace Equisetum2
{
	static String MakeNativePath(const String& type, const String& id, const String& ext )
	{
		// "type/id" �̕���������
		// ����ID�Ɋg���q���t���Ă��Ă���������
		return Path::GetFullPath(type + "/" + Path::GetFileNameWithoutExtension(id) + ext);
	}

	static std::shared_ptr<Sprite> _LoadSpriteImpl(const String& id, const std::function<bool(rapidjson::Document& doc)> cbDoc = nullptr)
	{
		EQ_DURING
		{
			// json�ǂݏo��
			rapidjson::Document doc;
			{
				auto jsonStream = FileStream::CreateFromPath(MakeNativePath("sprite", id, ".json"));
				if (!jsonStream)
				{
					EQ_THROW(String::Sprintf(u8"%s�̃I�[�v���Ɏ��s���܂����B", id.c_str()));
				}
				auto jsonSize = static_cast<size_t>(jsonStream->Length());
				std::vector<uint8_t> jsonTmp(jsonSize + 1);

				auto optRead = jsonStream->Read(jsonTmp.data(), jsonSize);
				if (!optRead ||
					*optRead != jsonSize)
				{
					EQ_THROW(u8"json�̓ǂݏo���Ɏ��s���܂����B");
				}
				jsonTmp[jsonSize] = 0;

				doc.Parse(reinterpret_cast<const char*>(jsonTmp.data()));

				bool error = doc.HasParseError();
				if (error)
				{
					size_t offset = doc.GetErrorOffset();
					rapidjson::ParseErrorCode code = doc.GetParseError();
					const char* msg = rapidjson::GetParseError_En(code);

					EQ_THROW(String::Sprintf(u8"json�̃p�[�X���ɃG���[���������܂����B%d:%d(%s)", static_cast<uint32_t>(offset), code, msg));
				}
			}

			// �^�C�v��v�`�F�b�N
			{
				// type�擾
				auto& type = doc.FindMember("type");
				if (type == doc.MemberEnd() ||
					type->value.GetType() != rapidjson::kStringType)
				{
					EQ_THROW(u8"type��������܂���B");
				}

				// �^�C�v��v����
				String strType = type->value.GetString();
				if (strType != "sprite")
				{
					EQ_THROW(u8"type��sprite�ł͂���܂���B");
				}
			}

			// ID��v�`�F�b�N
			{
				// id�擾
				auto& id_ = doc.FindMember("id");
				if (id_ == doc.MemberEnd() ||
					id_->value.GetType() != rapidjson::kStringType)
				{
					EQ_THROW(u8"id��������܂���B");
				}

				// ID��v����
				String strID = id_->value.GetString();
				if (strID != id)
				{
					EQ_THROW(u8"�w�肳�ꂽid�ƃt�@�C������id����v���܂���B");
				}
			}

			// �e�N�X�`���ǂݍ���
			std::shared_ptr<Texture> textureIn;
			{
				// �e�N�X�`��ID�擾
				auto& imageid = doc.FindMember("textureid");
				if (imageid == doc.MemberEnd() ||
					imageid->value.GetType() != rapidjson::kStringType)
				{
					EQ_THROW(u8"textureid��������܂���B");
				}

				// �e�N�X�`���ǂݍ���
				String strTextureID = imageid->value.GetString();
				textureIn = Singleton<AssetManager>::GetInstance()->Load<Texture>(strTextureID);
			}

			// �A�g���X�ǂݍ���
			std::vector<stSpriteAnimAtlas> vSpriteAnimAtlas;
			{
				auto& atlas = doc.FindMember("atlas");
				if (atlas == doc.MemberEnd() ||
					atlas->value.GetType() != rapidjson::kArrayType)
				{
					EQ_THROW(u8"atlas��������܂���B");
				}

				rapidjson::SizeType atlasArraySize = atlas->value.Size();
				for (decltype(atlasArraySize) i = 0; i < atlasArraySize; i++)
				{
					auto& elem = atlas->value[i];

					if (elem.GetType() == rapidjson::kObjectType)
					{
						struct info
						{
							Point point;			// �J�n���W
							Size src;				// �]�����T�C�Y
							PointF pivot;			// ���S���W
							Size matrix{ 1, 1 };	// �� x �c
						}tmp;

						auto& obj = atlas->value[i].GetObject();

						auto& pos = obj.FindMember("pos");
						if (pos != obj.MemberEnd())
						{
							if (pos->value.GetType() != rapidjson::kArrayType)
							{
								EQ_THROW(u8"pos�̏������s���ł��B");
							}

							if (pos->value.Size() != 2)
							{
								EQ_THROW(u8"pos�̗v�f�����s���ł��B");
							}

							if (pos->value[0].GetType() != rapidjson::kNumberType ||
								pos->value[1].GetType() != rapidjson::kNumberType)
							{
								EQ_THROW(u8"pos�̗v�f�͐����l�łȂ���΂����܂���B");
							}

							tmp.point.x = static_cast<int32_t>(pos->value[0].GetInt());
							tmp.point.y = static_cast<int32_t>(pos->value[1].GetInt());
							Logger::OutputDebug("  pos %d %d", tmp.point.x, tmp.point.y);
						}

						auto& size = obj.FindMember("size");
						if (size != obj.MemberEnd())
						{
							if (size->value.GetType() != rapidjson::kArrayType)
							{
								EQ_THROW(u8"size�̏������s���ł��B");
							}

							if (size->value.Size() != 2)
							{
								EQ_THROW(u8"size�̗v�f�����s���ł��B");
							}

							if (size->value[0].GetType() != rapidjson::kNumberType ||
								size->value[1].GetType() != rapidjson::kNumberType)
							{
								EQ_THROW(u8"size�̗v�f�͐����l�łȂ���΂����܂���B");
							}

							tmp.src.x = static_cast<int32_t>(size->value[0].GetInt());
							tmp.src.y = static_cast<int32_t>(size->value[1].GetInt());
							Logger::OutputDebug("  size %d %d", tmp.src.x, tmp.src.y);
						}

						auto& pivot = obj.FindMember("pivot");
						if (pivot != obj.MemberEnd())
						{
							if (pivot->value.GetType() != rapidjson::kArrayType)
							{
								EQ_THROW(u8"pivot�̏������s���ł��B");
							}

							if (pivot->value.Size() != 2)
							{
								EQ_THROW(u8"pivot�̗v�f�����s���ł��B");
							}

							if (pivot->value[0].GetType() != rapidjson::kNumberType ||
								pivot->value[1].GetType() != rapidjson::kNumberType)
							{
								EQ_THROW(u8"pivot�̗v�f�͏����l�łȂ���΂����܂���B");
							}

							tmp.pivot.x = pivot->value[0].GetFloat();
							tmp.pivot.y = pivot->value[1].GetFloat();
							Logger::OutputDebug("  pivot %lf %lf", tmp.pivot.x, tmp.pivot.y);
						}

						auto& matrix = obj.FindMember("matrix");
						if (matrix != obj.MemberEnd())
						{
							if (matrix->value.GetType() != rapidjson::kArrayType)
							{
								EQ_THROW(u8"matrix�̏������s���ł��B");
							}

							if (matrix->value.Size() != 2)
							{
								EQ_THROW(u8"matrix�̗v�f�����s���ł��B");
							}

							if (matrix->value[0].GetType() != rapidjson::kNumberType ||
								matrix->value[1].GetType() != rapidjson::kNumberType)
							{
								EQ_THROW(u8"matrix�̗v�f�͐����l�łȂ���΂����܂���B");
							}

							tmp.matrix.x = static_cast<int32_t>(matrix->value[0].GetInt());
							tmp.matrix.y = static_cast<int32_t>(matrix->value[1].GetInt());
							Logger::OutputDebug("  matrix %d %d", tmp.matrix.x, tmp.matrix.y);
						}

						for (auto y = 0; y < tmp.matrix.y; y++)
						{
							for (auto x = 0; x < tmp.matrix.x; x++)
							{
								stSpriteAnimAtlas anim{};

								anim.m_point.x = tmp.point.x + tmp.src.x * x;
								anim.m_point.y = tmp.point.y + tmp.src.y * y;

								anim.m_srcSize = tmp.src;
								anim.m_pivot = tmp.pivot;

								vSpriteAnimAtlas.emplace_back(anim);
							}
						}
					}
				}
			}

			Logger::OutputDebug("  vSpriteAnimAtlas size %d", vSpriteAnimAtlas.size());

			if (cbDoc)
			{
				// �ǉ����ڂ̃p�[�X�̓R�[���o�b�N�ōs��
				if (!cbDoc(doc))
				{
					EQ_THROW(u8"�ǉ����ڂ̉�͒��Ɏ��s���܂����B");
				}
			}

			auto sprite = Sprite::CreateFromTexture(textureIn);
			if (!sprite)
			{
				EQ_THROW(u8"Sprite�̍쐬�Ɏ��s���܂����B");
			}

			if (!sprite->SetAnimAtlas(vSpriteAnimAtlas))
			{
				EQ_THROW(u8"Sprite�̃A�g���X�o�^�Ɏ��s���܂����B");
			}

			// ID�ݒ�
			sprite->SetIdentify(id);

			return sprite;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return nullptr;
	}

	AssetManager::AssetManager()
	{
		Singleton<SharedPool<Image>>::GetInstance();
		Singleton<SharedPool<Sprite>>::GetInstance();
		Singleton<SharedPool<BitmapFont>>::GetInstance();
		Singleton<SharedPool<BGM>>::GetInstance();
		Singleton<SharedPool<SE>>::GetInstance();
		Singleton<SharedPool<Texture>>::GetInstance();
	}

	bool AssetManager::SetArchivePath(const String& path, const String& secretKey)
	{
		EQ_DURING
		{
			auto stream = FileStream::CreateFromPath(Path::GetFullPath(path));
			if (!stream)
			{
				EQ_THROW(String::Sprintf(u8"�A�[�J�C�u %s�̃I�[�v���Ɏ��s���܂����B", path.c_str()));
			}

			auto archiveStream = ArchiveAccessor::CreateFromStream(stream, secretKey);
			if (!archiveStream)
			{
				EQ_THROW(String::Sprintf(u8"�A�[�J�C�u%s�̉�͂Ɏ��s���܂����B", path.c_str()));
			}

			m_archiveStream = archiveStream;
			m_archivePath = path;
			m_secretKey = secretKey;

			return true;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return false;
	}

	void AssetManager::AllowUrlRewrite(bool allow)
	{
		m_allowUrlRewrite = allow;
	}

	std::shared_ptr<IStream> AssetManager::GetStreamByID(const String& type, const String& id, const String& ext)
	{
		EQ_DURING
		{
			if (m_allowUrlRewrite)
			{
				String nativePath = MakeNativePath(type, id, ext);

				if (File::Exists(nativePath))
				{
					auto fileStream = FileStream::CreateFromPath(nativePath);
					if (!fileStream)
					{
						EQ_THROW(String::Sprintf(u8"%s�̃I�[�v���Ɏ��s���܂����B", nativePath.c_str()));
					}

					return fileStream;
				}
			}

			if(!m_archiveStream)
			{
				EQ_THROW(u8"�A�[�J�C�u���I�[�v������Ă��܂���B");
			}

			String archiveID = type + "/" + id + ext;
			ArchiveMeta assetMeta;
			m_archiveStream->EnumerateFiles([&assetMeta, &archiveID](const ArchiveMeta& meta)->bool {
				if (archiveID == meta.id)
				{
					assetMeta = meta;
					return true;
				}

				return false;
			});

			if (assetMeta.id.empty())
			{
				EQ_THROW(String::Sprintf(u8"%s��������܂���B", archiveID.c_str()));
			}

			// �V�����X�g���[���Ƃ��ăA�[�J�C�u���J��
			auto fileStream = FileStream::CreateFromPath(Path::GetFullPath(m_archivePath));
			if (!fileStream)
			{
				EQ_THROW(String::Sprintf(u8"�A�[�J�C�u %s�̃I�[�v���Ɏ��s���܂����B", m_archivePath.c_str()));
			}

			return ArchiveAccessor::QuickLoadFromStream(fileStream, assetMeta, m_secretKey);
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return nullptr;
	}

	std::shared_ptr<Image> AssetManager::_LoadImage(const String& id)
	{
		EQ_DURING
		{
			// TODO: �摜�̌`����PNG�Œ肾���Ajpg��bmp�����ɍs���悤�ɂ���
#if 1
			auto inStream = FileStream::CreateFromPath(MakeNativePath("image", id, ".png"));
			if (!inStream)
			{
				EQ_THROW(String::Sprintf(u8"%s�̃I�[�v���Ɏ��s���܂����B", id.c_str()));
			}
#else
			auto inStream = GetStreamByID("image", id, ".png");
			if (!inStream)
			{
				EQ_THROW(String::Sprintf(u8"%s�̃I�[�v���Ɏ��s���܂����B", id.c_str()));
			}
#endif

			auto imageIn = Image::CreateFromStream(inStream);
			if (!imageIn)
			{
				EQ_THROW(String::Sprintf(u8"�C���[�W�t�@�C��(%s)�̃��[�h�Ɏ��s���܂����B", id.c_str()));
			}

			imageIn->SetIdentify(id);
			
			auto data = imageIn->Data();
			for (int i = 0; i < imageIn->Width() * imageIn->Height(); i++)
			{
				if (data[i].rgba8888.r == 0 &&
					data[i].rgba8888.g == 0xff &&
					data[i].rgba8888.b == 0)
				{
					data[i].rgba8888.a = 0;
				}
			}

			return imageIn;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return nullptr;
	}

	std::shared_ptr<Texture> AssetManager::_LoadTexture(const String& id)
	{
		// ���k�e�N�X�`�����̏����͂܂��l����
		// ����png�摜���Œ�Ō��ɍs��

		EQ_DURING
		{
			auto imageIn = _LoadImage(id);
			if (!imageIn)
			{
				EQ_THROW(u8"�e�N�X�`���̃��[�h�Ɏ��s���܂����B");
			}

			auto textureIn = Texture::CreateFromImage(imageIn);
			if (!textureIn)
			{
				EQ_THROW(String::Sprintf(u8"�e�N�X�`��(%s)�̍쐬�Ɏ��s���܂����B", id.c_str()));
			}

			textureIn->SetIdentify(id);

			return textureIn;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return nullptr;
	}

	std::shared_ptr<Sprite> AssetManager::_LoadSprite(const String& id)
	{
		return _LoadSpriteImpl(id);
	}

	std::shared_ptr<BitmapFont> AssetManager::_LoadBitmapFont(const String& id)
	{
		EQ_DURING
		{
			String codePoint;

			// �X�v���C�g��json�ǉ����ڂƂ���codepoint�����݂���̂ŁA������R�[���o�b�N�œǂݏo��
			auto cb = [&codePoint](rapidjson::Document& doc)->bool {
				auto& codepoint_ = doc.FindMember("codepoint");
				if (codepoint_ == doc.MemberEnd() ||
					codepoint_->value.GetType() != rapidjson::kStringType)
				{
					return false;
				}

				codePoint = codepoint_->value.GetString();

				return true;
			};

			// �X�v���C�g�����[�h����
			auto sprite = _LoadSpriteImpl(id, cb);
			if (!sprite)
			{
				EQ_THROW(String::Sprintf(u8"�X�v���C�g(%s)�̃��[�h�Ɏ��s���܂����B", id.c_str()));
			}

			// �C���X�^���X���쐬����
			auto bitmapFont = BitmapFont::CreateFromSprite(sprite, codePoint);
			if (!bitmapFont)
			{
				EQ_THROW(String::Sprintf(u8"BitmapFont(%s)�̃��[�h�Ɏ��s���܂����B", id.c_str()));
			}

			return bitmapFont;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return nullptr;
	}

	std::shared_ptr<FontManager> AssetManager::_LoadFontManager(const String& id)
	{
		EQ_DURING
		{
			// �b��I�� "/font/sample.ttf?16" �̂悤��id�ƃt�H���g�T�C�Y���w�肷��
			std::vector<String> str = id.split("?");

			auto inStream = FileStream::CreateFromPath(MakeNativePath("font", str[0], ".ttf"));
			if (!inStream)
			{
				EQ_THROW(String::Sprintf(u8"%s�̃I�[�v���Ɏ��s���܂����B", id.c_str()));
			}

			// �t�H���g�T�C�Y���w��
			int fontSize = 16;
			if (str.size() >= 2)
			{
				fontSize = atoi(str[1].c_str());
			}

			auto font = FontManager::CreateFromStream(inStream, fontSize);
			if (!font)
			{
				EQ_THROW(String::Sprintf(u8"�t�H���g(%s)�̍쐬�Ɏ��s���܂����B", id.c_str()));
			}

			font->SetIdentify(id);

			return font;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return nullptr;
	}

	std::shared_ptr<BGM> AssetManager::_LoadBGM(const String & id)
	{
		EQ_DURING
		{
			const String exts[] = { ".ogg", ".mp3", ".wav" };
			std::shared_ptr<FileStream> inStream;

			for (auto& ext : exts)
			{
				const String path = MakeNativePath("bgm", id, ext);

				if (File::Exists(path))
				{
					inStream = FileStream::CreateFromPath(path);
					if (!inStream)
					{
						EQ_THROW(String::Sprintf(u8"%s�̃I�[�v���Ɏ��s���܂����B", id.c_str()));
					}
					break;
				}
			}

			if (!inStream)
			{
				EQ_THROW(String::Sprintf(u8"%s�����݂��܂���B", id.c_str()));
			}

			auto bgm = BGM::CreateFromStream(inStream);
			if (!bgm)
			{
				EQ_THROW(String::Sprintf(u8"BGM(%s)�̃��[�h�Ɏ��s���܂����B", id.c_str()));
			}

			bgm->SetIdentify(id);

			return bgm;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return nullptr;
	}

	std::shared_ptr<SE> AssetManager::_LoadSE(const String & id)
	{
		EQ_DURING
		{
			const String exts[] = { ".ogg", ".mp3", ".wav" };
			std::shared_ptr<FileStream> inStream;

			for (auto& ext : exts)
			{
				const String path = MakeNativePath("se", id, ext);

				if (File::Exists(path))
				{
					inStream = FileStream::CreateFromPath(path);
					if (!inStream)
					{
						EQ_THROW(String::Sprintf(u8"%s�̃I�[�v���Ɏ��s���܂����B", id.c_str()));
					}
					break;
				}
			}

			if (!inStream)
			{
				EQ_THROW(String::Sprintf(u8"%s�����݂��܂���B", id.c_str()));
			}

			auto se = SE::CreateFromStream(inStream);
			if (!se)
			{
				EQ_THROW(String::Sprintf(u8"SE(%s)�̃��[�h�Ɏ��s���܂����B", id.c_str()));
			}

			se->SetIdentify(id);

			return se;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return nullptr;
	}
}

