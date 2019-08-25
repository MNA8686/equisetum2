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
	// "type/id" �̕���������
	// ����ID�Ɋg���q���t���Ă��Ă���������
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
		// �m�[�h�쐬
		NodeHandler nodeHandler = Node<Object>::GetFreeNodeWithInit(id);
		if (nodeHandler.id < 0)
		{
			EQ_THROW(u8"�m�[�h�̍쐬�Ɏ��s���܂����B");
		}
		// �I�u�W�F�N�g���擾
		Node<Object>* node = Node<Object>::GetNodeByHandler(nodeHandler);
		Object& attachedObject = node->GetAttach();
		stAsset* pAsset = attachedObject.GetAsset();
		if (!pAsset)
		{
			EQ_THROW(u8"�A�Z�b�g�̃}�b�s���O�Ɏ��s���܂����B");
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
			EQ_THROW(u8"�t�@�C���̃I�[�v���Ɏ��s���܂����B");
		}

		rapidjson::Document json = JsonParseHelper::CreateFromStream(stream);

		// object�̒�`�t�@�C�����ǂ����`�F�b�N
		{
			auto& it = json.FindMember("type");
			if (it == json.MemberEnd() ||
				!it->value.IsString())
			{
				EQ_THROW(u8"type��������܂���B");
			}

			String strType = it->value.GetString();
			if (strType != "object")
			{
				EQ_THROW(u8"type��object�ł͂���܂���B");
			}
		}

		// ID��v�`�F�b�N
		{
			// id�擾
			auto& id_ = json.FindMember("id");
			if (id_ == json.MemberEnd() ||
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

		// �A�Z�b�g���擾
		{
			auto& it = json.FindMember("asset");
			if (it == json.MemberEnd() ||
				!it->value.IsObject())
			{
				EQ_THROW(u8"asset��������܂���B");
			}

			auto object = it->value.GetObject();
			for (auto& obj : object)
			{
				if (obj.name == "sprite")
				{
					if(!obj.value.IsArray())
					{
						EQ_THROW(u8"sprite�͔z��łȂ���΂����܂���B");
					}

					for (auto& v : obj.value.GetArray())
					{
						if (!v.IsString())
						{
							EQ_THROW(u8"sprite���͕�����łȂ���΂����܂���B");
						}

						auto p = Singleton<AssetManager>::GetInstance()->Load<Sprite>(v.GetString());
						if (!p)
						{
							EQ_THROW(u8"sprite�̃��[�h�Ɏ��s���܂����B");
						}

						pAsset->m_sprite.push_back(p);

						Logger::OutputDebug(v.GetString());
					}
				}
				else if (obj.name == "bgm")
				{
					if (!obj.value.IsArray())
					{
						EQ_THROW(u8"bgm�͔z��łȂ���΂����܂���B");
					}

					for (auto& v : obj.value.GetArray())
					{
						if (!v.IsString())
						{
							EQ_THROW(u8"bgm���͕�����łȂ���΂����܂���B");
						}

						auto p = Singleton<AssetManager>::GetInstance()->Load<BGM>(v.GetString());
						if (!p)
						{
							EQ_THROW(u8"bgm�̃��[�h�Ɏ��s���܂����B");
						}

						pAsset->m_bgm.push_back(p);

						Logger::OutputDebug(v.GetString());
					}
				}
				else if (obj.name == "se")
				{
					if (!obj.value.IsArray())
					{
						EQ_THROW(u8"se�͔z��łȂ���΂����܂���B");
					}

					for (auto& v : obj.value.GetArray())
					{
						if (!v.IsString())
						{
							EQ_THROW(u8"se���͕�����łȂ���΂����܂���B");
						}

						auto p = Singleton<AssetManager>::GetInstance()->Load<SE>(v.GetString());
						if (!p)
						{
							EQ_THROW(u8"se�̃��[�h�Ɏ��s���܂����B");
						}

						pAsset->m_se.push_back(p);

						Logger::OutputDebug(v.GetString());
					}
				}
				else if (obj.name == "script")
				{
					if (!obj.value.IsArray())
					{
						EQ_THROW(u8"script�͔z��łȂ���΂����܂���B");
					}

					for (auto& v : obj.value.GetArray())
					{
						if (!v.IsString())
						{
							EQ_THROW(u8"script���͕�����łȂ���΂����܂���B");
						}

						auto p = Script::Create(v.GetString());
						if (!p)
						{
							EQ_THROW(u8"�X�N���v�g�̃��[�h�Ɏ��s���܂����B");
						}

						// ���L���Ă���I�u�W�F�N�g��ݒ肷��
						p->SetOwner(attachedObject.GetNodeHandler());
						// ID��ݒ肷��
						p->SetIdentify(v.GetString());

						pAsset->m_script.push_back(p);

						Logger::OutputDebug(v.GetString());
					}
				}
			}
		}

		// �X�N���v�g��OnCreate�Ăяo��
		for (auto& script : pAsset->m_script)
		{
			script->OnCreate();
		}

		// �č\�z�t���O�Z�b�g
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
		// ���[���h���W�X�V
		//-------------------------------------
		m_pos = pos;

		//-------------------------------------
		// ���[�J�����W�X�V
		//-------------------------------------
		{
			if (auto thisNode = Node<Object>::GetNodeByHandler(m_hNode))
			{
				// �e�ɒǏ]����ݒ� && �e�������Ă���H
				if (m_relativeParent &&
					thisNode->HasParent())
				{
					// �e�m�[�h�Ɋ֘A�t����ꂽObject���擾	
					if (auto parentObject = GetParent())
					{
						// �e�Ƃ̑��΍��W���Z�o
						m_localPos = m_pos - parentObject->GetPos();
					}
				}
				else
				{
					// m_pos��m_localPos�͓���
					m_localPos = pos;
				}
			}
		}

		// ���[���h���W���ω������̂Ŏq�ɓ`������
		SetPosForChild();
	}
}

void Object::SetPosForChild()
{
	// ���̃��\�b�h���Ăяo�����̂̓��[���h���W���ω������Ƃ��݂̂ł���B
	// ����āA�q�̃��[���h���W�͊m���ɍX�V����������B

	if (auto thisNode = Node<Object>::GetNodeByHandler(m_hNode))
	{
		if (thisNode->GetChildCount() > 0)
		{
			for (auto& childHandler : thisNode->GetChildrenHandler())
			{
				if (auto childObject = GetObjectByHandler(childHandler))
				{
					// �q�͐e�ɒǏ]����H
					if (childObject->GetRelativeParent())
					{
						// �q�̃��[���h���W�X�V
						childObject->m_pos = m_pos + childObject->GetLocalPos();

						// ���[���h���W���ω������̂Ŏq�ɓ`������
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
		// ���[�J�����W�X�V
		//-------------------------------------
		m_localPos = pos;

		//-------------------------------------
		// ���[���h���W�X�V
		//-------------------------------------
		{
			// �e�ɒǏ]����ݒ� && �e�������Ă���H
			if (m_relativeParent &&
				HasParent())
			{
				// �e�m�[�h�Ɋ֘A�t����ꂽObject���擾	
				if (auto parentObject = GetParent())
				{
					// �e�Ƃ̑��΍��W���烏�[���h���W���Z�o
					m_pos = parentObject->GetPos() + m_localPos;
				}
			}
			else
			{
				// m_pos��m_localPos�͓���
				m_pos = pos;
			}
		}

		// ���[���h���W���ω������̂Ŏq�ɓ`������
		SetPosForChild();
	}
}

bool Object::GetRelativeParent() const
{
	return m_relativeParent;
}

void Object::SetRelativeParent(bool on)
{
	// �����̃��[���h���W�ɕω��͖���(���̂��߁A���̃t���O���ω����Ă��q�ɓ`��������K�v�͂Ȃ�)
	// ���΍��W�݂̂��K�v�ɉ����ĕω�����
	m_relativeParent = on;

	//-------------------------------------
	// ���[�J�����W�X�V
	//-------------------------------------
	{
		// �e�ɒǏ]����ݒ� && �e�������Ă���H
		if (m_relativeParent &&
			HasParent())
		{
			// �e�m�[�h�Ɋ֘A�t����ꂽObject���擾	
			if (auto parentObject = GetParent())
			{
				// �e�Ƃ̑��΍��W���Z�o
				m_localPos = m_pos - parentObject->GetPos();
			}
		}
		else
		{
			// m_pos��m_localPos�͓���
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
				auto& object = node.GetAttach();		// �ǉ���������
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

			// �A�N�e�B�u���\����ԂłȂ���΂��̐�̃m�[�h�͏������Ȃ�
			if (!obj.m_active || !obj.m_visible)
			{
				return false;
			}

			if (stMappedResource* pMapped = Singleton<ResourceMapper>::GetInstance()->Map(obj.m_hNode.id))
			{
				// �\����Ԃ̃����_�[�I�u�W�F�N�g�������_�[�L���[�ɓ����
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

			// �č\�z�t���O�Z�b�g
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

		// �C���X�^���X�쐬
		auto tmpObject = std::make_shared<Object>();
		if (!tmpObject)
		{
			EQ_THROW(u8"�C���X�^���X�̍쐬�Ɏ��s���܂����B");
		}

		// �m�[�h�쐬
		auto tmpNode = Node<Object>::Create(thisNode->GetName());
		if (!tmpNode)
		{
			EQ_THROW(u8"�m�[�h�̍쐬�Ɏ��s���܂����B");
		}
		// �I�u�W�F�N�g���A�^�b�`
		tmpNode->SetAttach(tmpObject);

		tmpObject->m_asset.m_bgm = m_asset.m_bgm;			/// �A�Z�b�g�Ǘ��\����
		tmpObject->m_asset.m_se = m_asset.m_se;				/// �A�Z�b�g�Ǘ��\����
		tmpObject->m_asset.m_sprite = m_asset.m_sprite;		/// �A�Z�b�g�Ǘ��\����
//		std::vector<std::shared_ptr<RenderObject>> m_vRenderObject;	/// �����_�[�I�u�W�F�N�g�z��
		tmpObject->m_pos = m_pos;		/// ���[���h���W
		tmpObject->m_localPos = m_localPos;	/// �e�Ƃ̑��΍��W�B�e�����Ȃ����A�܂��͐e�ɒǏ]���Ȃ�����m_pos�Ɠ����B
		tmpObject->m_relativeParent = m_relativeParent;	/// �e�̍��W�ɒǏ]���邩�ǂ���
										//	int32_t m_angle = 0;
		tmpObject->m_active = m_active;			/// false�̏ꍇ�A�X�N���v�g�Ȃǂ��Ăяo����Ȃ�
		tmpObject->m_visible = m_visible;			/// false�̏ꍇ�A�����_�����O�ΏۂƂȂ�Ȃ�

		tmpObject->SetParent(GetParent());

		// �X�N���v�g���擾
		for (auto& script : m_asset.m_script)
		{
			auto p = Script::Create(script->Identify());
			if (!p)
			{
				EQ_THROW(String::Sprintf(u8"�X�N���v�g(%s)�̃��[�h�Ɏ��s���܂����B", script->Identify().c_str()));
			}

			// ���L���Ă���I�u�W�F�N�g��ݒ肷��
			p->SetOwner(tmpObject);
			p->SetIdentify(script->Identify());

			tmpObject->m_asset.m_script.push_back(p);

			Logger::OutputDebug(String::Sprintf(u8"�X�N���v�g(%s)���[�h�����B", script->Identify().c_str()).c_str());
		}

		// �X�N���v�g��OnCreate�Ăяo��
		for (auto& script : tmpObject->m_asset.m_script)
		{
			script->OnCreate();
		}

		// �č\�z�t���O�Z�b�g
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
