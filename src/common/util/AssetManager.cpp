#include "system/Exception.hpp"
#include "system/Logger.h"
#include "util/AssetManager.hpp"
#include "util/SharedPool.hpp"
#include "stream/FileStream.h"
#include "fs/Path.hpp"
#include "fs/File.hpp"
#include "fs/Directory.hpp"

#include "cereal/external/rapidjson/document.h"
#include "cereal/external/rapidjson/reader.h"
#include "cereal/external/rapidjson/error/error.h"
#include "cereal/external/rapidjson/error/en.h"

namespace Equisetum2
{
	static const String gSystemPath = "system/";
	static const String gSystemFile = "system.eq2";

	static String MakeIdPath(const String& type, const String& id, const String& ext )
	{
		// system�t�H���_���w�肳��Ă���ꍇ�́Asystem�t�H���_�����̃p�X���쐬����
		if (id.compare(0, gSystemPath.size(), gSystemPath) == 0)
		{
			return gSystemPath + type + "/" + Path::ChangeExtension(id.substr(gSystemPath.size()), "") + ext;
		}
		else
		{
			// "type/id" �̕���������
			// ����ID�Ɋg���q���t���Ă��Ă���������
			return type + "/" + Path::ChangeExtension(id, "") + ext;
		}
	}

	static std::shared_ptr<Sprite> _LoadSpriteImpl(const String& id, const std::function<bool(rapidjson::Document& doc)> cbDoc = nullptr)
	{
		EQ_DURING
		{
			// json�ǂݏo��
			rapidjson::Document doc;
			{
				String idPath = MakeIdPath("sprite", id, ".json");
	
				auto jsonStream = Singleton<AssetManager>::GetInstance()->GetStreamByID(idPath);
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
				auto type = doc.FindMember("type");
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

			// �e�N�X�`���ǂݍ���
			std::shared_ptr<Texture> textureIn;
			{
				// �e�N�X�`��ID�擾
				auto imageid = doc.FindMember("textureid");
				if (imageid == doc.MemberEnd() ||
					imageid->value.GetType() != rapidjson::kStringType)
				{
					EQ_THROW(u8"textureid��������܂���B");
				}

				// �e�N�X�`���ǂݍ���
				String strTextureID = imageid->value.GetString();
				textureIn = Singleton<AssetManager>::GetInstance()->Load<Texture>(strTextureID);
				if (!textureIn)
				{
					EQ_THROW(u8"�e�N�X�`���̃��[�h�Ɏ��s���܂����B");
				}
			}

			// �A�g���X�ǂݍ���
			std::vector<stSpriteAnimAtlas> vSpriteAnimAtlas;
			typedef struct
			{
				String tag;
				int32_t index;
			}stTag;
			std::vector<stTag> vSpriteTag;

			{
				auto atlas = doc.FindMember("atlas");
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

						auto obj = atlas->value[i].GetObject();

						auto tag = obj.FindMember("tag");
						if (tag != obj.MemberEnd())
						{
							// �e�N�X�`���ǂݍ���
							String strTag = tag->value.GetString();
							stTag tagInfo{ strTag, static_cast<int32_t>(vSpriteAnimAtlas.size()) };
							vSpriteTag.push_back(tagInfo);

							Logger::OutputDebug("  tag %s %d", tagInfo.tag.c_str(), tagInfo.index);
						}

						auto pos = obj.FindMember("pos");
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

						auto size = obj.FindMember("size");
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

						auto pivot = obj.FindMember("pivot");
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

						auto matrix = obj.FindMember("matrix");
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

			for (auto& tag : vSpriteTag)
			{
				if (!sprite->SetTag(tag.tag, tag.index))
				{
					EQ_THROW(u8"Sprite�̃^�O�o�^�Ɏ��s���܂����B");
					break;
				}
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
		Singleton<SharedPool<Animation>>::GetInstance();
	}

	bool AssetManager::SetArchivePath(const String& path, const String& secretKey)
	{
		EQ_DURING
		{
			/* �A�[�J�C�u���I�[�v������ */
			auto stream = FileStream::CreateFromPath(Path::GetFullPath(path));
			if (!stream)
			{
				EQ_THROW(String::Sprintf(u8"�A�[�J�C�u %s�̃I�[�v���Ɏ��s���܂����B", path.c_str()));
			}

			/* �I�[�v�������X�g���[�������^���ǂݏo���p�Ɏg�p����B
			   �����ɕ����̃A�Z�b�g���J�����Ƃ����邽�߁A�t�@�C���̒��g��
			   ���̓s�x�X�g���[���𐶐����Ďg�p����B */
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

	bool AssetManager::ExistsByID( const String & id)
	{
		EQ_DURING
		{
			if (m_allowUrlRewrite)
			{
				// fs��̃t�@�C����T��
				if (File::Exists(Path::GetFullPath(id)))
				{
					return true;
				}
			}

			if (!m_archiveStream)
			{
				if (m_allowUrlRewrite)
				{
					return false;
				}

				EQ_THROW(u8"�A�[�J�C�u���I�[�v������Ă��܂���B");
			}

			// �A�[�J�C�u���̃t�@�C����T��
			ArchiveMeta assetMeta;
			m_archiveStream->EnumerateFiles([&assetMeta, &id](const ArchiveMeta& meta)->bool {
				if (id == meta.id)
				{
					assetMeta = meta;
					return true;
				}

				return false;
			});

			return !assetMeta.id.empty();
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return false;
	}

	std::shared_ptr<IStream> AssetManager::GetStreamByID(const String& id)
	{
		EQ_DURING
		{
			// system�t�H���_���̃t�@�C�����w�肳��Ă���H
			if (id.compare(0, gSystemPath.size(), gSystemPath) == 0)
			{
				// �A�[�J�C�u���I�[�v������
				auto stream = FileStream::CreateFromPath(Path::GetFullPath(gSystemFile));
				if (!stream)
				{
					EQ_THROW(String::Sprintf(u8"�A�[�J�C�u %s�̃I�[�v���Ɏ��s���܂����B", gSystemFile.c_str()));
				}

				/* �I�[�v�������X�g���[�������^���ǂݏo���p�Ɏg�p����B
				   �����ɕ����̃A�Z�b�g���J�����Ƃ����邽�߁A�t�@�C���̒��g��
				   ���̓s�x�X�g���[���𐶐����Ďg�p����B */
				auto archiveStream = ArchiveAccessor::CreateFromStream(stream, "EquinoxDevelopment");
				if (!archiveStream)
				{
					EQ_THROW(String::Sprintf(u8"�A�[�J�C�u%s�̉�͂Ɏ��s���܂����B", gSystemFile.c_str()));
				}

				auto fileStream = archiveStream->FindByID(id.substr(gSystemPath.size()));
				if (!fileStream)
				{
					EQ_THROW(String::Sprintf(u8"�V�X�e���t�@�C��%s�̎擾�Ɏ��s���܂����B", id.c_str()));
				}

				return fileStream;
			}

			if (m_allowUrlRewrite)
			{
				String nativePath = Path::GetFullPath(id);

				if (File::Exists(nativePath))
				{
					// fs��̃t�@�C�����J��
					auto fileStream = FileStream::CreateFromPath(nativePath);
					if (!fileStream)
					{
						EQ_THROW(String::Sprintf(u8"%s�̃I�[�v���Ɏ��s���܂����B", nativePath.c_str()));
					}

					return fileStream;
				}
			}

			if (!m_archiveStream)
			{
				if (m_allowUrlRewrite)
				{
					return nullptr;
				}

				EQ_THROW(u8"�A�[�J�C�u���I�[�v������Ă��܂���B");
			}

			// �A�[�J�C�u���̃t�@�C�����J��
			ArchiveMeta assetMeta;
			m_archiveStream->EnumerateFiles([&assetMeta, &id](const ArchiveMeta& meta)->bool {
				if (id == meta.id)
				{
					assetMeta = meta;
					return true;
				}

				return false;
			});

			if (assetMeta.id.empty())
			{
				EQ_THROW(String::Sprintf(u8"%s��������܂���B", id.c_str()));
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
	
	std::vector<String> AssetManager::GetIdList(const String& type)
	{
		std::vector<String> vID;

		EQ_DURING
		{
			if (m_allowUrlRewrite)
			{
				String fullPath = Path::GetFullPath(type);

				// fs��̃t�@�C���ꗗ���擾����
				if (Directory::Exists(fullPath))
				{
					// �t���p�X�̐��K�����s��
					if (fullPath[fullPath.size() - 1] != Path::DirectorySeparatorChar[0])
					{
						fullPath += Path::DirectorySeparatorChar;
					}

					// �f�B���N�g�����̃t�@�C�����ċA�I�ɗ񋓂���
					auto optFiles = Directory::GetFiles(fullPath, true);
					if (optFiles)
					{
						vID = std::move(*optFiles);

						// �����p�X����菜���Ă��f�B���N�g���̋�؂蕶�����X���b�V���ɂ���
						for (auto& filename : vID)
						{
							filename = filename.substr(fullPath.size());

							if (Path::DirectorySeparatorChar != "/")
							{
								for (auto& c : filename)
								{
									if (c == Path::DirectorySeparatorChar[0])
									{
										c = '/';
									}
								}
							}
						}
					}
				}
			}

			if (m_archiveStream)
			{
				// �A�[�J�C�u�̒��̃t�@�C���ꗗ���擾����
				const String cmpType = type + "/";
				m_archiveStream->EnumerateFiles([&type, &vID, &cmpType](const ArchiveMeta& meta)->bool {
					if (meta.id.compare(0, cmpType.size(), cmpType) == 0)
					{
						vID.push_back(meta.id.substr(cmpType.size()));
					}

					return false;
				});
			}
			else
			{
				if (!m_allowUrlRewrite)
				{
					EQ_THROW(u8"�A�[�J�C�u���I�[�v������Ă��܂���B");
				}
			}
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return vID;
	}

	std::shared_ptr<Image> AssetManager::_LoadImage(const String& id)
	{
		EQ_DURING
		{
			// TODO: �摜�̌`����PNG�Œ肾���Ajpg��bmp�����ɍs���悤�ɂ���
			String idPath = MakeIdPath("image", id, ".png");
			auto inStream = GetStreamByID(idPath);
			if (!inStream)
			{
				EQ_THROW(String::Sprintf(u8"%s�̃I�[�v���Ɏ��s���܂����B", id.c_str()));
			}

			auto imageIn = Image::CreateFromStream(inStream);
			if (!imageIn)
			{
				EQ_THROW(String::Sprintf(u8"�C���[�W�t�@�C��(%s)�̃��[�h�Ɏ��s���܂����B", id.c_str()));
			}

			imageIn->SetIdentify(id);
			
			auto data = imageIn->Data();
			for (uint32_t i = 0; i < imageIn->Width() * imageIn->Height(); i++)
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
				auto codepoint_ = doc.FindMember("codepoint");
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

			// �V���A���C�Y�p�q���g��ݒ肷��
			BitmapFont::SerializeHint hint;
			hint.from = BitmapFont::SerializeHint::From::Asset;
			hint.id = id;
			bitmapFont->SetHint(hint);

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

			String idPath = MakeIdPath("font", str[0], ".ttf");
			
			auto inStream = GetStreamByID(idPath);
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
			std::shared_ptr<IStream> inStream;

			for (auto& ext : exts)
			{
				String idPath = MakeIdPath("bgm", id, ext);

				if (ExistsByID(idPath))
				{
					inStream = GetStreamByID(idPath);
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
			std::shared_ptr<IStream> inStream;

			for (auto& ext : exts)
			{
				String idPath = MakeIdPath("se", id, ext);

				if (ExistsByID(idPath))
				{
					inStream = GetStreamByID(idPath);
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

	std::shared_ptr<Animation> AssetManager::_LoadAnimation(const String& id)
	{
		EQ_DURING
		{
			// json�ǂݏo��
			rapidjson::Document doc;
			{
				String idPath = MakeIdPath("animation", id, ".json");
	
				auto jsonStream = Singleton<AssetManager>::GetInstance()->GetStreamByID(idPath);
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

			// �A�j���[�V�����R���e�i�쐬
			auto animationObj = Animation::Create();

			// �^�C�v��v�`�F�b�N
			{
				// type�擾
				auto type = doc.FindMember("type");
				if (type == doc.MemberEnd() ||
					type->value.GetType() != rapidjson::kStringType)
				{
					EQ_THROW(u8"type��������܂���B");
				}

				// �^�C�v��v����
				String strType = type->value.GetString();
				if (strType != "animation")
				{
					EQ_THROW(u8"type��animation�ł͂���܂���B");
				}
			}

			// parse "ptr"
			auto ptr = doc.FindMember("ptr");
			if (ptr == doc.MemberEnd() ||
				ptr->value.GetType() != rapidjson::kArrayType)
			{
				EQ_THROW(u8"ptr��������܂���B");
			}
			for (auto& elem : ptr->value.GetArray())
			{
				if (elem.GetType() != rapidjson::kObjectType)
				{
					EQ_THROW(u8"ptr�̔z����ɂ�object���z�u����Ă��Ȃ���΂����܂���B");
				}
				auto obj = elem.GetObject();

				// Animation�I�u�W�F�N�g�쐬
				auto animation = AnimationTimeline::Create();

				animation->Begin();

				// parse "tag"
				auto tag = obj.FindMember("tag");
				if (tag == obj.MemberEnd() ||
					tag->value.GetType() != rapidjson::kStringType)
				{
					EQ_THROW(u8"tag��������܂���B");
				}

				// parse "looptype"
				auto looptype = obj.FindMember("looptype");
				if (looptype == obj.MemberEnd() ||
					looptype->value.GetType() != rapidjson::kStringType)
				{
					EQ_THROW(u8"looptype��������܂���B");
				}
				String strLoopType = looptype->value.GetString();
				if (strLoopType != "none" &&
					strLoopType != "loop" &&
					strLoopType != "pingPong")
				{
					EQ_THROW(u8"�s����looptype���ݒ肳��Ă��܂��B");
				}
				// �A�j���[�V�����^�C�v��ݒ�
				if (strLoopType == "loop")
				{
					animation->SetLoopType(AnimationLoopType::loop);
				}
				else if (strLoopType == "pingPong")
				{
					animation->SetLoopType(AnimationLoopType::pingPong);
				}

				// parse "rotate"
				auto rotate = obj.FindMember("rotate");
				if (rotate != obj.MemberEnd())
				{
					if (rotate->value.GetType() != rapidjson::kObjectType)
					{
						EQ_THROW(u8"rotate��object�łȂ���΂����܂���B");
					}

					auto rotateObj = rotate->value.GetObject();

					AnimationTimeline::stRotate rotate;

					// parse "step"
					auto step = rotateObj.FindMember("step");
					if (step != rotateObj.MemberEnd() &&
						step->value.GetType() == rapidjson::kNumberType)
					{
						rotate.step = step->value.GetInt();
					}

					// parse "offset"
					auto offset = rotateObj.FindMember("offset");
					if (offset != rotateObj.MemberEnd() &&
						offset->value.GetType() == rapidjson::kNumberType)
					{
						rotate.offset = offset->value.GetInt();
					}

					animation->SetRotate(rotate);
				}

				// for "timeline" parse
				typedef struct
				{
					std::shared_ptr<Sprite> sprite;
					String tag;
					int32_t range[2] = { 0 };
					int32_t delay = 0;
				}stTimelineElem;
				stTimelineElem timelineElem;

				// parse "timeline"
				auto timeline = obj.FindMember("timeline");
				if (timeline == obj.MemberEnd() ||
					timeline->value.GetType() != rapidjson::kArrayType)
				{
					EQ_THROW(u8"timeline��������܂���B");
				}
				for (auto& elemTL : timeline->value.GetArray())
				{
					if (elemTL.GetType() != rapidjson::kObjectType)
					{
						EQ_THROW(u8"timeline�̔z����ɂ�object���z�u����Ă��Ȃ���΂����܂���B");
					}

					auto objTL = elemTL.GetObject();

					// parse "sprite"
					auto sprite = objTL.FindMember("sprite");
					if (sprite == objTL.MemberEnd() ||
						sprite->value.GetType() != rapidjson::kStringType)
					{
						EQ_THROW(u8"sprite��������܂���B");
					}
					// �X�v���C�g�ǂݍ���
					String strSpriteID = sprite->value.GetString();
					timelineElem.sprite = Singleton<AssetManager>::GetInstance()->Load<Sprite>(strSpriteID);
					if (!timelineElem.sprite)
					{
						EQ_THROW(u8"sprite�̃��[�h�Ɏ��s���܂����B");
					}

					// parse "tag"
					auto tag = objTL.FindMember("tag");
					if (tag == objTL.MemberEnd() ||
						tag->value.GetType() != rapidjson::kStringType)
					{
						EQ_THROW(u8"tag��������܂���B");
					}
					timelineElem.tag = tag->value.GetString();

					// parse "range"
					auto range = objTL.FindMember("range");
					if (range == objTL.MemberEnd() ||
						range->value.GetType() != rapidjson::kArrayType)
					{
						EQ_THROW(u8"range��������܂���B");
					}
					if (range->value.Size() != 2)
					{
						EQ_THROW(u8"range�̗v�f����2�łȂ���΂����܂���B");
					}
					if (range->value[0].GetType() != rapidjson::kNumberType ||
						range->value[1].GetType() != rapidjson::kNumberType)
					{
						EQ_THROW(u8"range�̗v�f�͐����l�łȂ���΂����܂���B");
					}
					timelineElem.range[0] = static_cast<int32_t>(range->value[0].GetInt());
					timelineElem.range[1] = static_cast<int32_t>(range->value[1].GetInt());

					// parse "delay"
					auto delay = objTL.FindMember("delay");
					if (delay == objTL.MemberEnd() ||
						delay->value.GetType() != rapidjson::kNumberType)
					{
						EQ_THROW(u8"delay��������܂���B");
					}
					timelineElem.delay = static_cast<int32_t>(delay->value.GetInt());


					// �A�j���[�V�����p�^�[����o�^����
					int32_t targetValue = 0;
					if (timelineElem.range[1] == -1)
					{
						if (timelineElem.tag.empty())
						{
							targetValue = static_cast<int32_t>(timelineElem.sprite->GetAnimAtlas().size());
						}
						else
						{
							int32_t tagIndex = timelineElem.sprite->TagToInt(timelineElem.tag);
							targetValue = timelineElem.sprite->GetAtlasSizeByTagIndex(tagIndex);
						}
					}
					else
					{
						targetValue = timelineElem.range[0] + timelineElem.range[1];
					}

					// �����͈̔͂��w�肳��Ă���ꍇ�͊J���ēo�^����
					for (int32_t i = timelineElem.range[0]; i < targetValue; i++)
					{
						animation->AppendTimeline(timelineElem.sprite, timelineElem.tag, i, timelineElem.delay);
					}

				}

				animation->End();

				animationObj->AppendAnimation(tag->value.GetString(), animation);
			}

			// ID�ݒ�
			animationObj->SetIdentify(id);

			return animationObj;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return nullptr;
	}
}

