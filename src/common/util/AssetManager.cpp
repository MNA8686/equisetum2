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
		// "type/id" の文字列を作る
		// 入力IDに拡張子が付いていても無視する
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
			// TODO: 画像の形式はPNG固定だが、jpgとbmpも見に行くようにする
			auto inStream = FileStream::CreateFromPath(MakeNativePath("image", id, ".png"));
			if (!inStream)
			{
				EQ_THROW(String::Sprintf(u8"%sのオープンに失敗しました。", id.c_str()));
			}

			auto imageIn = Image::CreateFromStream(inStream);
			if (!imageIn)
			{
				EQ_THROW(String::Sprintf(u8"イメージファイル(%s)のロードに失敗しました。", id.c_str()));
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
		// 圧縮テクスチャ時の処理はまた考える
		// 今はpng画像を固定で見に行く

		EQ_DURING
		{
			auto imageIn = _LoadImage(id);
			if (!imageIn)
			{
				EQ_THROW(u8"テクスチャのロードに失敗しました。");
			}

			auto textureIn = Texture::CreateFromImage(imageIn);
			if (!textureIn)
			{
				EQ_THROW(String::Sprintf(u8"テクスチャ(%s)の作成に失敗しました。", id.c_str()));
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
			// json読み出し
			rapidjson::Document doc;
			{
				auto jsonStream = FileStream::CreateFromPath(MakeNativePath("sprite", id, ".json"));
				if (!jsonStream)
				{
					EQ_THROW(String::Sprintf(u8"%sのオープンに失敗しました。", id.c_str()));
				}
				auto jsonSize = static_cast<size_t>(jsonStream->Length());
				std::vector<uint8_t> jsonTmp(jsonSize + 1);

				auto optRead = jsonStream->Read(jsonTmp.data(), jsonSize);
				if (!optRead ||
					*optRead != jsonSize)
				{
					EQ_THROW(u8"jsonの読み出しに失敗しました。");
				}
				jsonTmp[jsonSize] = 0;

				doc.Parse(reinterpret_cast<const char*>(jsonTmp.data()));

				bool error = doc.HasParseError();
				if (error)
				{
					size_t offset = doc.GetErrorOffset();
					rapidjson::ParseErrorCode code = doc.GetParseError();
					const char* msg = rapidjson::GetParseError_En(code);

					EQ_THROW(String::Sprintf(u8"jsonのパース時にエラーが発生しました。%d:%d(%s)", static_cast<uint32_t>(offset), code, msg));
				}
			}

			// タイプ一致チェック
			{
				// type取得
				auto& type = doc.FindMember("type");
				if (type == doc.MemberEnd() ||
					type->value.GetType() != rapidjson::kStringType)
				{
					EQ_THROW(u8"typeが見つかりません。");
				}

				// タイプ一致判定
				String strType = type->value.GetString();
				if (strType != "sprite")
				{
					EQ_THROW(u8"typeがspriteではありません。");
				}
			}

			// ID一致チェック
			{
				// id取得
				auto& id_ = doc.FindMember("id");
				if (id_ == doc.MemberEnd() ||
					id_->value.GetType() != rapidjson::kStringType)
				{
					EQ_THROW(u8"idが見つかりません。");
				}

				// ID一致判定
				String strID = id_->value.GetString();
				if(strID != id)
				{
					EQ_THROW(u8"指定されたidとファイル内のidが一致しません。");
				}
			}

			// テクスチャ読み込み
			std::shared_ptr<Texture> textureIn;
			{
				// テクスチャID取得
				auto& imageid = doc.FindMember("textureid");
				if (imageid == doc.MemberEnd() ||
					imageid->value.GetType() != rapidjson::kStringType)
				{
					EQ_THROW(u8"textureidが見つかりません。");
				}

				// テクスチャ読み込み
				String strTextureID = imageid->value.GetString();
				textureIn = _Load<Texture>(strTextureID);
			}

			// アトラス読み込み
			std::vector<stSpriteAnimAtlas> vSpriteAnimAtlas;
			{
				auto& atlas = doc.FindMember("atlas");
				if (atlas == doc.MemberEnd() ||
					atlas->value.GetType() != rapidjson::kArrayType)
				{
					EQ_THROW(u8"atlasが見つかりません。");
				}

				rapidjson::SizeType atlasArraySize = atlas->value.Size();
				for (decltype(atlasArraySize) i=0; i < atlasArraySize; i++)
				{
					auto& elem = atlas->value[i];

					if (elem.GetType() == rapidjson::kObjectType)
					{
						struct info
						{
							Point point;			// 開始座標
							Size src;				// 転送元サイズ
							PointF pivot;			// 中心座標
							Size matrix{ 1, 1 };	// 横 x 縦
						}tmp;

						auto& obj = atlas->value[i].GetObject();

						auto& pos = obj.FindMember("pos");
						if (pos != obj.MemberEnd())
						{
							if (pos->value.GetType() != rapidjson::kArrayType)
							{
								EQ_THROW(u8"posの書式が不正です。");
							}

							if (pos->value.Size() != 2)
							{
								EQ_THROW(u8"posの要素数が不正です。");
							}

							if (pos->value[0].GetType() != rapidjson::kNumberType ||
								pos->value[1].GetType() != rapidjson::kNumberType)
							{
								EQ_THROW(u8"posの要素は整数値でなければいけません。");
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
								EQ_THROW(u8"sizeの書式が不正です。");
							}

							if (size->value.Size() != 2)
							{
								EQ_THROW(u8"sizeの要素数が不正です。");
							}

							if (size->value[0].GetType() != rapidjson::kNumberType ||
								size->value[1].GetType() != rapidjson::kNumberType)
							{
								EQ_THROW(u8"sizeの要素は整数値でなければいけません。");
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
								EQ_THROW(u8"pivotの書式が不正です。");
							}

							if (pivot->value.Size() != 2)
							{
								EQ_THROW(u8"pivotの要素数が不正です。");
							}

							if (pivot->value[0].GetType() != rapidjson::kNumberType ||
								pivot->value[1].GetType() != rapidjson::kNumberType)
							{
								EQ_THROW(u8"pivotの要素は小数値でなければいけません。");
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
								EQ_THROW(u8"matrixの書式が不正です。");
							}

							if (matrix->value.Size() != 2)
							{
								EQ_THROW(u8"matrixの要素数が不正です。");
							}

							if (matrix->value[0].GetType() != rapidjson::kNumberType ||
								matrix->value[1].GetType() != rapidjson::kNumberType)
							{
								EQ_THROW(u8"matrixの要素は整数値でなければいけません。");
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
				EQ_THROW(u8"Spriteの作成に失敗しました。");
			}

			if (!sprite->SetAnimAtlas(vSpriteAnimAtlas))
			{
				EQ_THROW(u8"Spriteのアトラス登録に失敗しました。");
			}

			// ID設定
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
				EQ_THROW(String::Sprintf(u8"%sのオープンに失敗しました。", id.c_str()));
			}

			auto bgm = BGM::CreateFromStream(inStream);
			if (!bgm)
			{
				EQ_THROW(String::Sprintf(u8"BGM(%s)のロードに失敗しました。", id.c_str()));
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
				EQ_THROW(String::Sprintf(u8"%sのオープンに失敗しました。", id.c_str()));
			}

			auto se = SE::CreateFromStream(inStream);
			if (!se)
			{
				EQ_THROW(String::Sprintf(u8"SE(%s)のロードに失敗しました。", id.c_str()));
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

