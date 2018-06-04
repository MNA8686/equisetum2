#include "Object.hpp"
#include "Script.hpp"
#include "cereal/external/rapidjson/document.h"
#include "cereal/external/rapidjson/reader.h"
#include "cereal/external/rapidjson/error/error.h"
#include "cereal/external/rapidjson/error/en.h"

using namespace Equisetum2;

class JsonParseHelper
{
public:
	static rapidjson::Document CreateFromStream(std::shared_ptr<IStream> stream);
};

rapidjson::Document JsonParseHelper::CreateFromStream(std::shared_ptr<IStream> inStream)
{
	EQ_DURING
	{
		rapidjson::Document doc;
		auto jsonSize = static_cast<size_t>(inStream->Length());
		std::vector<uint8_t> jsonTmp(jsonSize + 1);

		auto optRead = inStream->Read(jsonTmp.data(), jsonSize);
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

		return std::move(doc);
	}
	EQ_HANDLER
	{
		Logger::OutputError(EQ_GET_HANDLER().what());
	}
	EQ_END_HANDLER

	return nullptr;
}


Object::Object()
{
}

Object::~Object()
{
}

std::shared_ptr<Object> Object::Create(const String& name)
{
	EQ_DURING
	{
		String id = "subaru.json";		// テスト用

		// インスタンス作成
		auto tmpNode = std::make_shared<Object>();
		if (!tmpNode)
		{
			EQ_THROW(u8"インスタンスの作成に失敗しました。");
		}

		if (!Node::Init(tmpNode, name))
		{
			EQ_THROW(u8"インスタンスの初期化に失敗しました。");
		}

		/*
			R"({
				"type" : "object",
				"name" : "subaru",
				"asset" :
				{
					"sprite": ["/sprite/subaru.json"],
					"se" : ["/se/subaru_shot.wav"],
					"script" : ["subaru_shot"]
				}
			})";
		*/
		rapidjson::Document json = JsonParseHelper::CreateFromStream(FileStream::CreateFromPath(id));

		// objectの定義ファイルかどうかチェック
		{
			auto& it = json.FindMember("type");
			if (it == json.MemberEnd() ||
				!it->value.IsString())
			{
				EQ_THROW(u8"typeが見つかりません。");
			}

			String strType = it->value.GetString();
			if (strType != "object")
			{
				EQ_THROW(u8"typeがobjectではありません。");
			}
		}

		// 名前を取得
		{
			auto& it = json.FindMember("name");
			if (it == json.MemberEnd() ||
				!it->value.IsString())
			{
				EQ_THROW(u8"nameが見つかりません。");
			}

			String strName = it->value.GetString();
			if (strName.empty())
			{
				EQ_THROW(u8"nameが設定されていません。");
			}
			if (strName != name)
			{
//				EQ_THROW(u8"nameが異なります。");
			}
		}

		// アセットを取得
		{
			auto& it = json.FindMember("asset");
			if (it == json.MemberEnd() ||
				!it->value.IsObject())
			{
				EQ_THROW(u8"assetが見つかりません。");
			}

			auto object = it->value.GetObject();
			for (auto& obj : object)
			{
				if (obj.name == "sprite")
				{
					if(!obj.value.IsArray())
					{
						EQ_THROW(u8"spriteは配列でなければいけません。");
					}

					for (auto& v : obj.value.GetArray())
					{
						if (!v.IsString())
						{
							EQ_THROW(u8"sprite名は文字列でなければいけません。");
						}

						auto p = Singleton<AssetManager>::GetInstance()->Load<Sprite>(v.GetString());
						if (!p)
						{
							EQ_THROW(u8"spriteのロードに失敗しました。");
						}

						tmpNode->m_asset.m_sprite.push_back(p);

						Logger::OutputError(v.GetString());
					}
				}
				else if (obj.name == "bgm")
				{
					if (!obj.value.IsArray())
					{
						EQ_THROW(u8"bgmは配列でなければいけません。");
					}

					for (auto& v : obj.value.GetArray())
					{
						if (!v.IsString())
						{
							EQ_THROW(u8"bgm名は文字列でなければいけません。");
						}

						auto p = Singleton<AssetManager>::GetInstance()->Load<BGM>(v.GetString());
						if (!p)
						{
							EQ_THROW(u8"bgmのロードに失敗しました。");
						}

						tmpNode->m_asset.m_bgm.push_back(p);

						Logger::OutputError(v.GetString());
					}
				}
				else if (obj.name == "se")
				{
					if (!obj.value.IsArray())
					{
						EQ_THROW(u8"seは配列でなければいけません。");
					}

					for (auto& v : obj.value.GetArray())
					{
						if (!v.IsString())
						{
							EQ_THROW(u8"se名は文字列でなければいけません。");
						}

						auto p = Singleton<AssetManager>::GetInstance()->Load<SE>(v.GetString());
						if (!p)
						{
							EQ_THROW(u8"seのロードに失敗しました。");
						}

						tmpNode->m_asset.m_se.push_back(p);

						Logger::OutputError(v.GetString());
					}
				}
				else if (obj.name == "script")
				{
					if (!obj.value.IsArray())
					{
						EQ_THROW(u8"scriptは配列でなければいけません。");
					}

					for (auto& v : obj.value.GetArray())
					{
						if (!v.IsString())
						{
							EQ_THROW(u8"script名は文字列でなければいけません。");
						}

						auto p = Script::Create(v.GetString());
						if (!p)
						{
							EQ_THROW(u8"スクリプトのロードに失敗しました。");
						}

						tmpNode->m_asset.m_script.push_back(p);

						Logger::OutputError(v.GetString());
					}
				}
			}
		}

		// スクリプトのOnCreate呼び出し
		for (auto& script : tmpNode->m_asset.m_script)
		{
			script->OnCreate();
		}

		return tmpNode;
	}
	EQ_HANDLER
	{
		Logger::OutputError(EQ_GET_HANDLER().what());
	}
	EQ_END_HANDLER

	return nullptr;
}

const Point_t<FixedDec>& Object::GetPos() const
{
	return m_pos;
}

const Point_t<FixedDec>& Object::GetLocalPos() const
{
	return m_localPos;
}

void Object::SetPos(const Point_t<FixedDec>& pos)
{
	if (m_pos != pos)
	{
		//-------------------------------------
		// ワールド座標更新
		//-------------------------------------
		m_pos = pos;

		//-------------------------------------
		// ローカル座標更新
		//-------------------------------------
		{
			// 親に追従する設定 && 親を持っている？
			if (m_relativeParent &&
				HasParent())
			{
				// 親ノード取得	
				std::shared_ptr<Node>& parentNode = GetNodeByID(GetParentID());
				if (auto p = std::dynamic_pointer_cast<Object>(parentNode))
				{
					// 親との相対座標を算出
					m_localPos = m_pos - p->GetPos();
				}
			}
			else
			{
				// m_posとm_localPosは等価
				m_localPos = pos;
			}
		}

		// ワールド座標が変化したので子に伝搬する
		SetPosForChild();
	}
}

void Object::SetPosForChild()
{
	// このメソッドが呼び出されるのはワールド座標が変化したときのみである。
	// よって、子のワールド座標は確実に更新が発生する。

	if (GetChildCount() > 0)
	{
		for (auto& child : GetChildrenID())
		{
			std::shared_ptr<Node>& childNode = GetNodeByID(child);
			if (auto p = std::dynamic_pointer_cast<Object>(childNode))
			{
				// 子は親に追従する？
				if (p->GetRelativeParent())
				{
					// 子のワールド座標更新
					p->m_pos = m_pos + p->GetLocalPos();

					// ワールド座標が変化したので子に伝搬する
					p->SetPosForChild();
				}
			}
		}
	}
}

void Object::SetLocalPos(const Point_t<FixedDec>& pos)
{
	if (m_localPos != pos)
	{
		//-------------------------------------
		// ローカル座標更新
		//-------------------------------------
		m_localPos = pos;

		//-------------------------------------
		// ワールド座標更新
		//-------------------------------------
		{
			// 親に追従する設定 && 親を持っている？
			if (m_relativeParent &&
				HasParent())
			{
				// 親ノード取得	
				std::shared_ptr<Node>& parentNode = GetNodeByID(GetParentID());
				if (auto p = std::dynamic_pointer_cast<Object>(parentNode))
				{
					// 親との相対座標からワールド座標を算出
					m_pos = p->GetPos() + m_localPos;
				}
			}
			else
			{
				// m_posとm_localPosは等価
				m_pos = pos;
			}
		}

		// ワールド座標が変化したので子に伝搬する
		SetPosForChild();
	}
}

bool Object::GetRelativeParent() const
{
	return m_relativeParent;
}

void Object::SetRelativeParent(bool on)
{
	// 自分のワールド座標に変化は無い(そのため、このフラグが変化しても子に伝搬させる必要はない)
	// 相対座標のみが必要に応じて変化する
	m_relativeParent = on;

	//-------------------------------------
	// ローカル座標更新
	//-------------------------------------
	{
		// 親に追従する設定 && 親を持っている？
		if (m_relativeParent &&
			HasParent())
		{
			// 親ノード取得	
			std::shared_ptr<Node>& parentNode = GetNodeByID(GetParentID());
			if (auto p = std::dynamic_pointer_cast<Object>(parentNode))
			{
				// 親との相対座標を算出
				m_localPos = m_pos - p->GetPos();
			}
		}
		else
		{
			// m_posとm_localPosは等価
			m_localPos = m_pos;
		}
	}
}
