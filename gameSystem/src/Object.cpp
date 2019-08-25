#include "Object.hpp"
#include "Node.hpp"
#include "Script.hpp"
#include "ResourceMapper.hpp"
#include "cereal/external/rapidjson/document.h"
#include "cereal/external/rapidjson/reader.h"
#include "cereal/external/rapidjson/error/error.h"
#include "cereal/external/rapidjson/error/en.h"

using namespace Equisetum2;

bool Object::m_dirty = true;

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
	Singleton<ResourceMapper>::GetInstance()->Unmap(m_hNode.id);
}

NodeHandler Object::Create(const String& id)
{
	EQ_DURING
	{
		// ノード作成
		NodeHandler nodeHandler = Node<Object>::GetFreeNodeWithInit(id);
		if (nodeHandler.id < 0)
		{
			EQ_THROW(u8"ノードの作成に失敗しました。");
		}
		// オブジェクトを取得
		Node<Object>* node = Node<Object>::GetNodeByHandler(nodeHandler);
		Object& attachedObject = node->GetAttach();
		stAsset* pAsset = attachedObject.GetAsset();
		if (!pAsset)
		{
			EQ_THROW(u8"アセットのマッピングに失敗しました。");
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
		auto stream = Singleton<AssetManager>::GetInstance()->GetStreamByID("object/" + id + ".json");
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

						pAsset->m_sprite.push_back(p);

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

						pAsset->m_bgm.push_back(p);

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

						pAsset->m_se.push_back(p);

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
						p->SetOwner(attachedObject.GetNodeHandler());
						// IDを設定する
						p->SetIdentify(v.GetString());

						pAsset->m_script.push_back(p);

						Logger::OutputDebug(v.GetString());
					}
				}
			}
		}

		// スクリプトのOnCreate呼び出し
		for (auto& script : pAsset->m_script)
		{
			script->OnCreate();
		}

		// 再構築フラグセット
		m_dirty = true;

		return nodeHandler;
	}
	EQ_HANDLER
	{
		Logger::OutputError(EQ_GET_HANDLER().what());
	}
	EQ_END_HANDLER

	return{};
}

#if 0
NodeID Object::CreateChild(const String& id)
{
	NodeID childID = Object::Create(id);
	if (childID >= 0)
	{
		auto& childObject = GetObjectByID(childID);
		childObject.SetParentID(m_nodeHandler);
	}

	return childID;
}
#endif

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
			if (auto thisNode = Node<Object>::GetNodeByHandler(m_hNode))
			{
				// 親に追従する設定 && 親を持っている？
				if (m_relativeParent &&
					thisNode->HasParent())
				{
					// 親ノードに関連付けられたObjectを取得	
					if (auto parentObject = GetParent())
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
		}

		// ワールド座標が変化したので子に伝搬する
		SetPosForChild();
	}
}

void Object::SetPosForChild()
{
	// このメソッドが呼び出されるのはワールド座標が変化したときのみである。
	// よって、子のワールド座標は確実に更新が発生する。

	if (auto thisNode = Node<Object>::GetNodeByHandler(m_hNode))
	{
		if (thisNode->GetChildCount() > 0)
		{
			for (auto& childHandler : thisNode->GetChildrenHandler())
			{
				if (auto childObject = GetObjectByHandler(childHandler))
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
				if (auto parentObject = GetParent())
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
			if (auto parentObject = GetParent())
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

bool Object::OnFixedUpdate()
{
	stAsset* pAsset = GetAsset();
	if (pAsset)
	{
		auto& vScript = pAsset->m_script;
		for (auto& script : vScript)
		{
			script->FixedUpdate();
		}
		return true;
	}

	return false;
}

void Object::Update()
{
	if (m_dirty)
	{
		Node<Object>::Reschedule([](Node<Object>& node)->bool {
				auto& object = node.GetAttach();		// 追加条件判定
				if (object.IsActive())
				{
					return true;
				}
				return false;
			});

		m_dirty = false;
	}

	const auto& schedule = Node<Object>::GetSchedule();
	for (const auto& id : schedule)
	{
		if (auto obj = GetObjectByHandler(id))
		{
			obj->OnFixedUpdate();
		}
	}
}

void Object::AddRenderObject(std::shared_ptr<RenderObject> renderObject)
{
	if (stMappedResource* pMapped = Singleton<ResourceMapper>::GetInstance()->Map(m_hNode.id))
	{
		pMapped->renderObject.push_back(renderObject);
	}
}

bool Object::OnDraw(std::shared_ptr<Renderer>& renderer)
{
	if (auto thisNode = Node<Object>::GetNodeByHandler(m_hNode))
	{
		Node<Object>::Visit(*thisNode, [this, &renderer](Node<Object>& node, int32_t nestDepth)->bool {
			auto& obj = node.GetAttach();

			// アクティブかつ表示状態でなければこの先のノードは処理しない
			if (!obj.m_active || !obj.m_visible)
			{
				return false;
			}

			if (stMappedResource* pMapped = Singleton<ResourceMapper>::GetInstance()->Map(obj.m_hNode.id))
			{
				// 表示状態のレンダーオブジェクトをレンダーキューに入れる
				for (auto& renderObject : pMapped->renderObject)
				{
					if (renderObject->IsVisible())
					{
						renderer->AddRenderQueue(renderObject.get());
					}
				}
			}

			return true;
		});
	}

	return true;
}

stAsset* Object::GetAsset()
{
	if (stMappedResource* pMapped = Singleton<ResourceMapper>::GetInstance()->Map(m_hNode.id))
	{
		return &pMapped->asset;
	}
	
	return nullptr;
}

Object* Object::GetObjectByHandler(const NodeHandler& handler)
{
	if (auto node = Node<Object>::GetNodeByHandler(handler))
	{
		return &node->GetAttach();
	}

	return nullptr;
}

Object* Object::Self()
{
	return GetObjectByHandler(m_hNode);
}

bool Object::HasParent() const
{
	if (auto node = Node<Object>::GetNodeByHandler(m_hNode))
	{
		return node->HasParent();
	}

	return false;
}

NodeHandler Object::GetParentHandler() const
{
	if (auto* selfNode = Node<Object>::GetNodeByHandler(m_hNode))
	{
		return selfNode->GetParentHandler();
	}

	return{};
}

Object* Object::GetParent() const
{
	return GetObjectByHandler(GetParentHandler());
}

void Object::Destroy()
{
	if (auto thisNode = Node<Object>::GetNodeByHandler(m_hNode))
	{
		if (!thisNode->IsDestroyed())
		{
			thisNode->Destroy();

			// 再構築フラグセット
			m_dirty = true;
		}
	}
}

bool Object::IsDestroyed() const
{
	if (auto thisNode = Node<Object>::GetNodeByHandler(m_hNode))
	{
		return thisNode->IsDestroyed();
	}

	return false;
}

void Object::SetParentHandler(const NodeHandler& newParent)
{
	if (auto thisNode = Node<Object>::GetNodeByHandler(m_hNode))
	{
		thisNode->SetParentHandler(newParent);
	}

	m_dirty = true;
}

void Object::DetachChildren()
{
	if (auto thisNode = Node<Object>::GetNodeByHandler(m_hNode))
	{
		thisNode->DetachChildren();
	}

	m_dirty = true;
}

const EqVector<NodeHandler>& Object::GetChildrenHandler() const
{
	auto thisNode = Node<Object>::GetNodeByHandler(m_hNode);
	return thisNode->GetChildrenHandler();
}

int32_t Object::GetChildCount() const
{
	if (auto thisNode = Node<Object>::GetNodeByHandler(m_hNode))
	{
		return thisNode->GetChildCount();
	}

	return 0;
}

bool Object::IsActive() const
{
	return m_active;
}

bool Object::IsVisible() const
{
	return m_visible;
}

void Object::SetActive(bool active)
{
	if (m_active != active)
	{
		m_dirty = true;
	}

	m_active = active;
}

void Object::SetVisible(bool visible)
{
	m_visible = visible;
}

std::shared_ptr<Object> Object::Fork()
{
#if 0
	EQ_DURING
	{
		auto& thisNode = Node<Object>::GetNodeByID(m_nodeID);

		// インスタンス作成
		auto tmpObject = std::make_shared<Object>();
		if (!tmpObject)
		{
			EQ_THROW(u8"インスタンスの作成に失敗しました。");
		}

		// ノード作成
		auto tmpNode = Node<Object>::Create(thisNode->GetName());
		if (!tmpNode)
		{
			EQ_THROW(u8"ノードの作成に失敗しました。");
		}
		// オブジェクトをアタッチ
		tmpNode->SetAttach(tmpObject);

		tmpObject->m_asset.m_bgm = m_asset.m_bgm;			/// アセット管理構造体
		tmpObject->m_asset.m_se = m_asset.m_se;				/// アセット管理構造体
		tmpObject->m_asset.m_sprite = m_asset.m_sprite;		/// アセット管理構造体
//		std::vector<std::shared_ptr<RenderObject>> m_vRenderObject;	/// レンダーオブジェクト配列
		tmpObject->m_pos = m_pos;		/// ワールド座標
		tmpObject->m_localPos = m_localPos;	/// 親との相対座標。親がいない時、または親に追従しない時はm_posと同じ。
		tmpObject->m_relativeParent = m_relativeParent;	/// 親の座標に追従するかどうか
										//	int32_t m_angle = 0;
		tmpObject->m_active = m_active;			/// falseの場合、スクリプトなどが呼び出されない
		tmpObject->m_visible = m_visible;			/// falseの場合、レンダリング対象とならない

		tmpObject->SetParent(GetParent());

		// スクリプトを取得
		for (auto& script : m_asset.m_script)
		{
			auto p = Script::Create(script->Identify());
			if (!p)
			{
				EQ_THROW(String::Sprintf(u8"スクリプト(%s)のロードに失敗しました。", script->Identify().c_str()));
			}

			// 所有しているオブジェクトを設定する
			p->SetOwner(tmpObject);
			p->SetIdentify(script->Identify());

			tmpObject->m_asset.m_script.push_back(p);

			Logger::OutputDebug(String::Sprintf(u8"スクリプト(%s)ロード成功。", script->Identify().c_str()).c_str());
		}

		// スクリプトのOnCreate呼び出し
		for (auto& script : tmpObject->m_asset.m_script)
		{
			script->OnCreate();
		}

		// 再構築フラグセット
		m_dirty = true;

		return tmpObject;
	}
	EQ_HANDLER
	{
		Logger::OutputError(EQ_GET_HANDLER().what());
	}
	EQ_END_HANDLER
#endif

	return nullptr;
}
