#include "Object.hpp"
#include "Node.hpp"
#include "Script.hpp"
#include "cereal/external/rapidjson/document.h"
#include "cereal/external/rapidjson/reader.h"
#include "cereal/external/rapidjson/error/error.h"
#include "cereal/external/rapidjson/error/en.h"

using namespace Equisetum2;

static std::shared_ptr<Object> nullObject;

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

static String MakeNativePath(const String& type, const String& id, const String& ext)
{
	// "type/id" の文字列を作る
	// 入力IDに拡張子が付いていても無視する
	return Path::GetFullPath(type + "/" + Path::GetFileNameWithoutExtension(id) + ext);
}

Object::Object()
{
}

Object::~Object()
{
}

std::shared_ptr<Object> Object::Create(const String& id)
{
	EQ_DURING
	{
		// インスタンス作成
		auto tmpObject = std::make_shared<Object>();
		if (!tmpObject)
		{
			EQ_THROW(u8"インスタンスの作成に失敗しました。");
		}
	
		/*
			R"({
				"type" : "object",
				"name" : "subaru",
				"asset" :
				{
					"sprite": ["subaru.json"],
					"se" : ["subaru_shot"],
					"script" : ["subaru_shot"]
				}
			})";
		*/
		auto stream = FileStream::CreateFromPath(MakeNativePath("object", id, ".json"));
		if (!stream)
		{
			EQ_THROW(u8"ファイルのオープンに失敗しました。");
		}

		rapidjson::Document json = JsonParseHelper::CreateFromStream(stream);

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

		// ID一致チェック
		{
			// id取得
			auto& id_ = json.FindMember("id");
			if (id_ == json.MemberEnd() ||
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

						tmpObject->m_asset.m_sprite.push_back(p);

						Logger::OutputDebug(v.GetString());
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

						tmpObject->m_asset.m_bgm.push_back(p);

						Logger::OutputDebug(v.GetString());
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

						tmpObject->m_asset.m_se.push_back(p);

						Logger::OutputDebug(v.GetString());
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

						// 所有しているオブジェクトを設定する
						p->SetOwner(tmpObject);

						tmpObject->m_asset.m_script.push_back(p);

						Logger::OutputDebug(v.GetString());
					}
				}
			}
		}

		// スクリプトのOnCreate呼び出し
		for (auto& script : tmpObject->m_asset.m_script)
		{
			script->OnCreate();
		}

		return tmpObject;
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
			auto& thisNode = Node<Object>::GetNodeByID(m_nodeID);

			// 親に追従する設定 && 親を持っている？
			if (m_relativeParent &&
				thisNode->HasParent())
			{
				// 親ノードに関連付けられたObjectを取得	
				if(auto& parentObject = thisNode->GetParent()->GetAttach())
				{
					// 親との相対座標を算出
					m_localPos = m_pos - parentObject->GetPos();
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

	auto& thisNode = Node<Object>::GetNodeByID(m_nodeID);

	if (thisNode->GetChildCount() > 0)
	{
		for (auto& child : thisNode->GetChildrenID())
		{
			if (auto& childObject = GetObjectByID(child))
			{
				// 子は親に追従する？
				if (childObject->GetRelativeParent())
				{
					// 子のワールド座標更新
					childObject->m_pos = m_pos + childObject->GetLocalPos();

					// ワールド座標が変化したので子に伝搬する
					childObject->SetPosForChild();
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
				// 親ノードに関連付けられたObjectを取得	
				if (auto& parentObject = GetParent())
				{
					// 親との相対座標からワールド座標を算出
					m_pos = parentObject->GetPos() + m_localPos;
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
			// 親ノードに関連付けられたObjectを取得	
			if (auto& parentObject = GetParent())
			{
				// 親との相対座標を算出
				m_localPos = m_pos - parentObject->GetPos();
			}
		}
		else
		{
			// m_posとm_localPosは等価
			m_localPos = m_pos;
		}
	}
}

void Object::AddRenderObject(std::shared_ptr<RenderObject> renderObject)
{
	m_vRenderObject.push_back(renderObject);
}

bool Object::OnDraw(std::shared_ptr<Renderer>& renderer)
{
	auto& thisNode = Node<Object>::GetNodeByID(m_nodeID);

	Node<Object>::Visit(thisNode, [this, &renderer](std::shared_ptr<Node<Object>>& node, int32_t nestDepth)->bool {
		auto& obj = node->GetAttach();

		// アクティブかつ表示状態でなければこの先のノードは処理しない
		if (!obj->m_active || !obj->m_visible)
		{
			return false;
		}

		// 表示状態のレンダーオブジェクトをレンダーキューに入れる
		for (auto& renderObject : obj->m_vRenderObject)
		{
			if (renderObject->IsVisible())
			{
				renderer->AddRenderQueue(renderObject.get());
			}
		}

		return true;
	});

	return true;
}

stAsset& Object::GetAsset()
{
	return m_asset;
}

void Object::SetNodeID(NodeID id)
{
	m_nodeID = id;
}

NodeID Object::GetNodeID() const
{
	return m_nodeID;
}

std::shared_ptr<Object>& Object::GetObjectByID(NodeID id)
{
	if (auto& node = Node<Object>::GetNodeByID(id))
	{
		return node->GetAttach();
	}

	return nullObject;
}

std::shared_ptr<Object>& Object::Self()
{
	return GetObjectByID(m_nodeID);
}

bool Object::HasParent() const
{
	return Node<Object>::GetNodeByID(m_nodeID)->HasParent();
}

std::shared_ptr<Object>& Object::GetParent()
{
	if (auto parentNode = Node<Object>::GetNodeByID(m_nodeID)->GetParent())
	{
		return parentNode->GetAttach();
	}

	return nullObject;
}
