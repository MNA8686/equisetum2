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
}

std::shared_ptr<Object> Object::Create(const String& id)
{
	EQ_DURING
	{
		// �C���X�^���X�쐬
		auto tmpNode = std::make_shared<Object>();
		if (!tmpNode)
		{
			EQ_THROW(u8"�C���X�^���X�̍쐬�Ɏ��s���܂����B");
		}

		if (!Node::Init(tmpNode, id))
		{
			EQ_THROW(u8"�C���X�^���X�̏������Ɏ��s���܂����B");
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

						tmpNode->m_asset.m_sprite.push_back(p);

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

						tmpNode->m_asset.m_bgm.push_back(p);

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

						tmpNode->m_asset.m_se.push_back(p);

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
						p->SetOwner(tmpNode);

						tmpNode->m_asset.m_script.push_back(p);

						Logger::OutputDebug(v.GetString());
					}
				}
			}
		}

		// �X�N���v�g��OnCreate�Ăяo��
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
		// ���[���h���W�X�V
		//-------------------------------------
		m_pos = pos;

		//-------------------------------------
		// ���[�J�����W�X�V
		//-------------------------------------
		{
			// �e�ɒǏ]����ݒ� && �e�������Ă���H
			if (m_relativeParent &&
				HasParent())
			{
				// �e�m�[�h�擾	
				std::shared_ptr<Node>& parentNode = GetNodeByID(GetParentID());
				if (auto p = std::dynamic_pointer_cast<Object>(parentNode))
				{
					// �e�Ƃ̑��΍��W���Z�o
					m_localPos = m_pos - p->GetPos();
				}
			}
			else
			{
				// m_pos��m_localPos�͓���
				m_localPos = pos;
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

	if (GetChildCount() > 0)
	{
		for (auto& child : GetChildrenID())
		{
			std::shared_ptr<Node>& childNode = GetNodeByID(child);
			if (auto p = std::dynamic_pointer_cast<Object>(childNode))
			{
				// �q�͐e�ɒǏ]����H
				if (p->GetRelativeParent())
				{
					// �q�̃��[���h���W�X�V
					p->m_pos = m_pos + p->GetLocalPos();

					// ���[���h���W���ω������̂Ŏq�ɓ`������
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
				// �e�m�[�h�擾	
				std::shared_ptr<Node>& parentNode = GetNodeByID(GetParentID());
				if (auto p = std::dynamic_pointer_cast<Object>(parentNode))
				{
					// �e�Ƃ̑��΍��W���烏�[���h���W���Z�o
					m_pos = p->GetPos() + m_localPos;
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
			// �e�m�[�h�擾	
			std::shared_ptr<Node>& parentNode = GetNodeByID(GetParentID());
			if (auto p = std::dynamic_pointer_cast<Object>(parentNode))
			{
				// �e�Ƃ̑��΍��W���Z�o
				m_localPos = m_pos - p->GetPos();
			}
		}
		else
		{
			// m_pos��m_localPos�͓���
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
#if 1
	std::shared_ptr<Node> pThis = Self();

	Visit(pThis, [this, &renderer](std::shared_ptr<Node>& node, int32_t nestDepth)->bool {
		auto obj = static_cast<Object*>(node.get());

		// �A�N�e�B�u���\����ԂłȂ���΂��̐�̃m�[�h�͏������Ȃ�
		if (!obj->m_active || !obj->m_visible)
		{
			return false;
		}

		// �\����Ԃ̃����_�[�I�u�W�F�N�g�������_�[�L���[�ɓ����
		for (auto& renderObject : obj->m_vRenderObject)
		{
			if (renderObject->IsVisible())
			{
				renderer->AddRenderQueue(renderObject.get());
			}
		}

		return true;
	});
#else
	// �A�N�e�B�u���\����ԁH
	if (m_active && m_visible)
	{
		// �\����Ԃ̃����_�[�I�u�W�F�N�g�������_�[�L���[�ɓ����
		for (auto& renderObject : m_vRenderObject)
		{
			if (renderObject->IsVisible())
			{
				renderer->AddRenderQueue(renderObject.get());
			}
		}

		// �q�ɓ`��������
		std::list<NodeID>& children = GetChildrenID();
		for (auto& id : children)
		{
			std::shared_ptr<Node>& child = GetNodeByID(id);
			if (child)
			{
				auto p = static_cast<Object*>(child.get());
				p->OnDraw(renderer);
			}
		}
	}
#endif

	return true;
}

stAsset& Object::GetAsset()
{
	return m_asset;
}

bool Object::AddScheduler()
{
	// �A�N�e�B�u�ȃm�[�h���X�P�W���[���ɓo�^����
	// �A�N�e�B�u�ł͂Ȃ��m�[�h�͂��̃m�[�h�̎q���o�^���Ȃ�
	return m_active;
}

bool Object::OnSchedule()
{
	for (auto& script : m_asset.m_script)
	{
		if (script->Start())
		{
			script->FixedUpdate();
		}
	}

	return true;
}
