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
		// "type/id" の文字列を作る
		// 入力IDに拡張子が付いていても無視する
		return Path::GetFullPath(type + "/" + Path::GetFileNameWithoutExtension(id) + ext);
	}

	static std::shared_ptr<Sprite> _LoadSpriteImpl(const String& id, const std::function<bool(rapidjson::Document& doc)> cbDoc = nullptr)
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
				if (strID != id)
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
				textureIn = Singleton<AssetManager>::GetInstance()->Load<Texture>(strTextureID);
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
				for (decltype(atlasArraySize) i = 0; i < atlasArraySize; i++)
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
				// 追加項目のパースはコールバックで行う
				if (!cbDoc(doc))
				{
					EQ_THROW(u8"追加項目の解析中に失敗しました。");
				}
			}

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
				EQ_THROW(String::Sprintf(u8"アーカイブ %sのオープンに失敗しました。", path.c_str()));
			}

			auto archiveStream = ArchiveAccessor::CreateFromStream(stream, secretKey);
			if (!archiveStream)
			{
				EQ_THROW(String::Sprintf(u8"アーカイブ%sの解析に失敗しました。", path.c_str()));
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
						EQ_THROW(String::Sprintf(u8"%sのオープンに失敗しました。", nativePath.c_str()));
					}

					return fileStream;
				}
			}

			if(!m_archiveStream)
			{
				EQ_THROW(u8"アーカイブがオープンされていません。");
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
				EQ_THROW(String::Sprintf(u8"%sが見つかりません。", archiveID.c_str()));
			}

			// 新しいストリームとしてアーカイブを開く
			auto fileStream = FileStream::CreateFromPath(Path::GetFullPath(m_archivePath));
			if (!fileStream)
			{
				EQ_THROW(String::Sprintf(u8"アーカイブ %sのオープンに失敗しました。", m_archivePath.c_str()));
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
			// TODO: 画像の形式はPNG固定だが、jpgとbmpも見に行くようにする
#if 1
			auto inStream = FileStream::CreateFromPath(MakeNativePath("image", id, ".png"));
			if (!inStream)
			{
				EQ_THROW(String::Sprintf(u8"%sのオープンに失敗しました。", id.c_str()));
			}
#else
			auto inStream = GetStreamByID("image", id, ".png");
			if (!inStream)
			{
				EQ_THROW(String::Sprintf(u8"%sのオープンに失敗しました。", id.c_str()));
			}
#endif

			auto imageIn = Image::CreateFromStream(inStream);
			if (!imageIn)
			{
				EQ_THROW(String::Sprintf(u8"イメージファイル(%s)のロードに失敗しました。", id.c_str()));
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
		return _LoadSpriteImpl(id);
	}

	std::shared_ptr<BitmapFont> AssetManager::_LoadBitmapFont(const String& id)
	{
		EQ_DURING
		{
			String codePoint;

			// スプライトのjson追加項目としてcodepointが存在するので、それをコールバックで読み出す
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

			// スプライトをロードする
			auto sprite = _LoadSpriteImpl(id, cb);
			if (!sprite)
			{
				EQ_THROW(String::Sprintf(u8"スプライト(%s)のロードに失敗しました。", id.c_str()));
			}

			// インスタンスを作成する
			auto bitmapFont = BitmapFont::CreateFromSprite(sprite, codePoint);
			if (!bitmapFont)
			{
				EQ_THROW(String::Sprintf(u8"BitmapFont(%s)のロードに失敗しました。", id.c_str()));
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
			// 暫定的に "/font/sample.ttf?16" のようにidとフォントサイズを指定する
			std::vector<String> str = id.split("?");

			auto inStream = FileStream::CreateFromPath(MakeNativePath("font", str[0], ".ttf"));
			if (!inStream)
			{
				EQ_THROW(String::Sprintf(u8"%sのオープンに失敗しました。", id.c_str()));
			}

			// フォントサイズを指定
			int fontSize = 16;
			if (str.size() >= 2)
			{
				fontSize = atoi(str[1].c_str());
			}

			auto font = FontManager::CreateFromStream(inStream, fontSize);
			if (!font)
			{
				EQ_THROW(String::Sprintf(u8"フォント(%s)の作成に失敗しました。", id.c_str()));
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
						EQ_THROW(String::Sprintf(u8"%sのオープンに失敗しました。", id.c_str()));
					}
					break;
				}
			}

			if (!inStream)
			{
				EQ_THROW(String::Sprintf(u8"%sが存在しません。", id.c_str()));
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
						EQ_THROW(String::Sprintf(u8"%sのオープンに失敗しました。", id.c_str()));
					}
					break;
				}
			}

			if (!inStream)
			{
				EQ_THROW(String::Sprintf(u8"%sが存在しません。", id.c_str()));
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

