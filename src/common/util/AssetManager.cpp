#include "system/Exception.hpp"
#include "system/Logger.h"
#include "util/AssetManager.hpp"
#include "util/SharedPool.hpp"
#include "stream/FileStream.h"
#include "fs/Path.hpp"

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

	AssetManager::AssetManager()
	{
		Singleton<SharedPool<Image>>::GetInstance();
		Singleton<SharedPool<Sprite>>::GetInstance();
		Singleton<SharedPool<BGM>>::GetInstance();
		Singleton<SharedPool<SE>>::GetInstance();
		Singleton<SharedPool<Texture>>::GetInstance();
	}

	std::shared_ptr<Image> AssetManager::_LoadImage(const String& id)
	{
		EQ_DURING
		{
			// TODO: �摜�̌`����PNG�Œ肾���Ajpg��bmp�����ɍs���悤�ɂ���
			auto inStream = FileStream::CreateFromPath(MakeNativePath("image", id, ".png"));
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
				if(strID != id)
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
				textureIn = _Load<Texture>(strTextureID);
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
				for (decltype(atlasArraySize) i=0; i < atlasArraySize; i++)
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
								anim.m_point.y = tmp.point.y + tmp.src.x * y;

								anim.m_srcSize = tmp.src;
								anim.m_pivot = tmp.pivot;

								vSpriteAnimAtlas.emplace_back(anim);
							}
						}
					}
				}
			}

			Logger::OutputDebug("  vSpriteAnimAtlas size %d", vSpriteAnimAtlas.size());

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

	std::shared_ptr<BGM> AssetManager::_LoadBGM(const String & id)
	{
		EQ_DURING
		{
			auto inStream = FileStream::CreateFromPath(MakeNativePath("bgm", id, ".ogg"));
			if (!inStream)
			{
				EQ_THROW(String::Sprintf(u8"%s�̃I�[�v���Ɏ��s���܂����B", id.c_str()));
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
			auto inStream = FileStream::CreateFromPath(MakeNativePath("se", id, ".wav"));
			if (!inStream)
			{
				EQ_THROW(String::Sprintf(u8"%s�̃I�[�v���Ɏ��s���܂����B", id.c_str()));
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

