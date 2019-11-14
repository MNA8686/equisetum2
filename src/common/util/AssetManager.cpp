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
		// systemフォルダが指定されている場合は、systemフォルダ向けのパスを作成する
		if (id.compare(0, gSystemPath.size(), gSystemPath) == 0)
		{
			return gSystemPath + type + "/" + Path::ChangeExtension(id.substr(gSystemPath.size()), "") + ext;
		}
		else
		{
			// "type/id" の文字列を作る
			// 入力IDに拡張子が付いていても無視する
			return type + "/" + Path::ChangeExtension(id, "") + ext;
		}
	}

	static std::shared_ptr<Sprite> _LoadSpriteImpl(const String& id, const std::function<bool(rapidjson::Document& doc)> cbDoc = nullptr)
	{
		EQ_DURING
		{
			// json読み出し
			rapidjson::Document doc;
			{
				String idPath = MakeIdPath("sprite", id, ".json");
	
				auto jsonStream = Singleton<AssetManager>::GetInstance()->GetStreamByID(idPath);
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
				auto type = doc.FindMember("type");
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

			// テクスチャ読み込み
			std::shared_ptr<Texture> textureIn;
			{
				// テクスチャID取得
				auto imageid = doc.FindMember("textureid");
				if (imageid == doc.MemberEnd() ||
					imageid->value.GetType() != rapidjson::kStringType)
				{
					EQ_THROW(u8"textureidが見つかりません。");
				}

				// テクスチャ読み込み
				String strTextureID = imageid->value.GetString();
				textureIn = Singleton<AssetManager>::GetInstance()->Load<Texture>(strTextureID);
				if (!textureIn)
				{
					EQ_THROW(u8"テクスチャのロードに失敗しました。");
				}
			}

			// アトラス読み込み
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

						auto obj = atlas->value[i].GetObject();

						auto tag = obj.FindMember("tag");
						if (tag != obj.MemberEnd())
						{
							// テクスチャ読み込み
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

						auto size = obj.FindMember("size");
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

						auto pivot = obj.FindMember("pivot");
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

						auto matrix = obj.FindMember("matrix");
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

			for (auto& tag : vSpriteTag)
			{
				if (!sprite->SetTag(tag.tag, tag.index))
				{
					EQ_THROW(u8"Spriteのタグ登録に失敗しました。");
					break;
				}
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
		Singleton<SharedPool<Animation>>::GetInstance();
	}

	bool AssetManager::SetArchivePath(const String& path, const String& secretKey)
	{
		EQ_DURING
		{
			/* アーカイブをオープンする */
			auto stream = FileStream::CreateFromPath(Path::GetFullPath(path));
			if (!stream)
			{
				EQ_THROW(String::Sprintf(u8"アーカイブ %sのオープンに失敗しました。", path.c_str()));
			}

			/* オープンしたストリームをメタ情報読み出し用に使用する。
			   同時に複数のアセットを開くことがあるため、ファイルの中身は
			   その都度ストリームを生成して使用する。 */
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

	bool AssetManager::ExistsByID( const String & id)
	{
		EQ_DURING
		{
			if (m_allowUrlRewrite)
			{
				// fs上のファイルを探す
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

				EQ_THROW(u8"アーカイブがオープンされていません。");
			}

			// アーカイブ内のファイルを探す
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
			// systemフォルダ内のファイルを指定されている？
			if (id.compare(0, gSystemPath.size(), gSystemPath) == 0)
			{
				// アーカイブをオープンする
				auto stream = FileStream::CreateFromPath(Path::GetFullPath(gSystemFile));
				if (!stream)
				{
					EQ_THROW(String::Sprintf(u8"アーカイブ %sのオープンに失敗しました。", gSystemFile.c_str()));
				}

				/* オープンしたストリームをメタ情報読み出し用に使用する。
				   同時に複数のアセットを開くことがあるため、ファイルの中身は
				   その都度ストリームを生成して使用する。 */
				auto archiveStream = ArchiveAccessor::CreateFromStream(stream, "EquinoxDevelopment");
				if (!archiveStream)
				{
					EQ_THROW(String::Sprintf(u8"アーカイブ%sの解析に失敗しました。", gSystemFile.c_str()));
				}

				auto fileStream = archiveStream->FindByID(id.substr(gSystemPath.size()));
				if (!fileStream)
				{
					EQ_THROW(String::Sprintf(u8"システムファイル%sの取得に失敗しました。", id.c_str()));
				}

				return fileStream;
			}

			if (m_allowUrlRewrite)
			{
				String nativePath = Path::GetFullPath(id);

				if (File::Exists(nativePath))
				{
					// fs上のファイルを開く
					auto fileStream = FileStream::CreateFromPath(nativePath);
					if (!fileStream)
					{
						EQ_THROW(String::Sprintf(u8"%sのオープンに失敗しました。", nativePath.c_str()));
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

				EQ_THROW(u8"アーカイブがオープンされていません。");
			}

			// アーカイブ内のファイルを開く
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
				EQ_THROW(String::Sprintf(u8"%sが見つかりません。", id.c_str()));
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
	
	std::vector<String> AssetManager::GetIdList(const String& type)
	{
		std::vector<String> vID;

		EQ_DURING
		{
			if (m_allowUrlRewrite)
			{
				String fullPath = Path::GetFullPath(type);

				// fs上のファイル一覧を取得する
				if (Directory::Exists(fullPath))
				{
					// フルパスの正規化を行う
					if (fullPath[fullPath.size() - 1] != Path::DirectorySeparatorChar[0])
					{
						fullPath += Path::DirectorySeparatorChar;
					}

					// ディレクトリ内のファイルを再帰的に列挙する
					auto optFiles = Directory::GetFiles(fullPath, true);
					if (optFiles)
					{
						vID = std::move(*optFiles);

						// 検索パスを取り除いてかつディレクトリの区切り文字をスラッシュにする
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
				// アーカイブの中のファイル一覧を取得する
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
					EQ_THROW(u8"アーカイブがオープンされていません。");
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
			// TODO: 画像の形式はPNG固定だが、jpgとbmpも見に行くようにする
			String idPath = MakeIdPath("image", id, ".png");
			auto inStream = GetStreamByID(idPath);
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
				auto codepoint_ = doc.FindMember("codepoint");
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

			// シリアライズ用ヒントを設定する
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
			// 暫定的に "/font/sample.ttf?16" のようにidとフォントサイズを指定する
			std::vector<String> str = id.split("?");

			String idPath = MakeIdPath("font", str[0], ".ttf");
			
			auto inStream = GetStreamByID(idPath);
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
			std::shared_ptr<IStream> inStream;

			for (auto& ext : exts)
			{
				String idPath = MakeIdPath("bgm", id, ext);

				if (ExistsByID(idPath))
				{
					inStream = GetStreamByID(idPath);
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
			std::shared_ptr<IStream> inStream;

			for (auto& ext : exts)
			{
				String idPath = MakeIdPath("se", id, ext);

				if (ExistsByID(idPath))
				{
					inStream = GetStreamByID(idPath);
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

	std::shared_ptr<Animation> AssetManager::_LoadAnimation(const String& id)
	{
		EQ_DURING
		{
			// json読み出し
			rapidjson::Document doc;
			{
				String idPath = MakeIdPath("animation", id, ".json");
	
				auto jsonStream = Singleton<AssetManager>::GetInstance()->GetStreamByID(idPath);
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

			// アニメーションコンテナ作成
			auto animationObj = Animation::Create();

			// タイプ一致チェック
			{
				// type取得
				auto type = doc.FindMember("type");
				if (type == doc.MemberEnd() ||
					type->value.GetType() != rapidjson::kStringType)
				{
					EQ_THROW(u8"typeが見つかりません。");
				}

				// タイプ一致判定
				String strType = type->value.GetString();
				if (strType != "animation")
				{
					EQ_THROW(u8"typeがanimationではありません。");
				}
			}

			// parse "ptr"
			auto ptr = doc.FindMember("ptr");
			if (ptr == doc.MemberEnd() ||
				ptr->value.GetType() != rapidjson::kArrayType)
			{
				EQ_THROW(u8"ptrが見つかりません。");
			}
			for (auto& elem : ptr->value.GetArray())
			{
				if (elem.GetType() != rapidjson::kObjectType)
				{
					EQ_THROW(u8"ptrの配列内にはobjectが配置されていなければいけません。");
				}
				auto obj = elem.GetObject();

				// Animationオブジェクト作成
				auto animation = AnimationTimeline::Create();

				animation->Begin();

				// parse "tag"
				auto tag = obj.FindMember("tag");
				if (tag == obj.MemberEnd() ||
					tag->value.GetType() != rapidjson::kStringType)
				{
					EQ_THROW(u8"tagが見つかりません。");
				}

				// parse "looptype"
				auto looptype = obj.FindMember("looptype");
				if (looptype == obj.MemberEnd() ||
					looptype->value.GetType() != rapidjson::kStringType)
				{
					EQ_THROW(u8"looptypeが見つかりません。");
				}
				String strLoopType = looptype->value.GetString();
				if (strLoopType != "none" &&
					strLoopType != "loop" &&
					strLoopType != "pingPong")
				{
					EQ_THROW(u8"不正なlooptypeが設定されています。");
				}
				// アニメーションタイプを設定
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
						EQ_THROW(u8"rotateはobjectでなければいけません。");
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
					EQ_THROW(u8"timelineが見つかりません。");
				}
				for (auto& elemTL : timeline->value.GetArray())
				{
					if (elemTL.GetType() != rapidjson::kObjectType)
					{
						EQ_THROW(u8"timelineの配列内にはobjectが配置されていなければいけません。");
					}

					auto objTL = elemTL.GetObject();

					// parse "sprite"
					auto sprite = objTL.FindMember("sprite");
					if (sprite == objTL.MemberEnd() ||
						sprite->value.GetType() != rapidjson::kStringType)
					{
						EQ_THROW(u8"spriteが見つかりません。");
					}
					// スプライト読み込み
					String strSpriteID = sprite->value.GetString();
					timelineElem.sprite = Singleton<AssetManager>::GetInstance()->Load<Sprite>(strSpriteID);
					if (!timelineElem.sprite)
					{
						EQ_THROW(u8"spriteのロードに失敗しました。");
					}

					// parse "tag"
					auto tag = objTL.FindMember("tag");
					if (tag == objTL.MemberEnd() ||
						tag->value.GetType() != rapidjson::kStringType)
					{
						EQ_THROW(u8"tagが見つかりません。");
					}
					timelineElem.tag = tag->value.GetString();

					// parse "range"
					auto range = objTL.FindMember("range");
					if (range == objTL.MemberEnd() ||
						range->value.GetType() != rapidjson::kArrayType)
					{
						EQ_THROW(u8"rangeが見つかりません。");
					}
					if (range->value.Size() != 2)
					{
						EQ_THROW(u8"rangeの要素数は2でなければいけません。");
					}
					if (range->value[0].GetType() != rapidjson::kNumberType ||
						range->value[1].GetType() != rapidjson::kNumberType)
					{
						EQ_THROW(u8"rangeの要素は整数値でなければいけません。");
					}
					timelineElem.range[0] = static_cast<int32_t>(range->value[0].GetInt());
					timelineElem.range[1] = static_cast<int32_t>(range->value[1].GetInt());

					// parse "delay"
					auto delay = objTL.FindMember("delay");
					if (delay == objTL.MemberEnd() ||
						delay->value.GetType() != rapidjson::kNumberType)
					{
						EQ_THROW(u8"delayが見つかりません。");
					}
					timelineElem.delay = static_cast<int32_t>(delay->value.GetInt());


					// アニメーションパターンを登録する
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

					// 複数の範囲が指定されている場合は開いて登録する
					for (int32_t i = timelineElem.range[0]; i < targetValue; i++)
					{
						animation->AppendTimeline(timelineElem.sprite, timelineElem.tag, i, timelineElem.delay);
					}

				}

				animation->End();

				animationObj->AppendAnimation(tag->value.GetString(), animation);
			}

			// ID設定
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

