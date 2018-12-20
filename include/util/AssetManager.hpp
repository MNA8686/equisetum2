#if !defined(_EQASSETMANAGER_H_)
#define _EQASSETMANAGER_H_

#include "util/SharedPool.hpp"
#include "util/ArchiveAccessor.hpp"
#include "system/Singleton.h"
#include "graphic/Image.hpp"
#include "graphic/Sprite.hpp"
#include "graphic/FontManager.hpp"
#include "graphic/BitmapFont.hpp"
#include "audio/BGM.hpp"
#include "audio/SE.hpp"

namespace Equisetum2
{
	class Sprite;
	class AssetManager final
	{
	public:
		friend Singleton<AssetManager>;	// �V���O���g������C���X�^���X���쐬���Ă��炦��悤�ɂ���

		template<typename T>
		std::shared_ptr<T> Load(const String& id, bool shared=true)
		{
			EQ_DURING
			{
				if (shared)
				{
					// ���łɃ��[�h�ς݂̃��\�[�X�Ȃ狤�L�|�C���^��Ԃ�
					if (auto fromPool = Singleton<SharedPool<T>>::GetInstance()->GetByIdentify(id))
					{
						return fromPool;
					}
				}

				// �V�K�Ƀ��[�h
				auto res = _Load<T>(id);
				if (!res)
				{
					EQ_THROW(String::Sprintf(u8"���\�[�X(%s)�̃��[�h�Ɏ��s���܂����B", id.c_str()));
				}

				if (!Singleton<SharedPool<T>>::GetInstance()->Register(id, res, shared))
				{
					EQ_THROW(String::Sprintf(u8"���\�[�X(%s)�̓o�^�Ɏ��s���܂����B", id.c_str()));
				}

				return res;
			}
			EQ_HANDLER
			{
				Logger::OutputError(EQ_GET_HANDLER().what());
			}
			EQ_END_HANDLER

			return nullptr;
		}
		
		// �A�[�J�C�u�̃p�X���w�肷��
		bool SetArchivePath(const String& path, const String& secretKey);
		// �A�Z�b�g�t�@�C�������ڔz�u����Ă���ꍇ�A�����̃t�@�C����ǂݏo�����Ƃ������邩�ǂ���
		void AllowUrlRewrite(bool allow);
		// �A�[�J�C�u���̃t�@�C���ւ̃X�g���[�����擾����
		std::shared_ptr<IStream> GetStreamByID(const String& id);
		// �A�[�J�C�u���Ƀt�@�C�������݂��邩�ǂ������擾����
		bool ExistsByID(const String& id);
		// �^�C�v���Ƃ�ID�ꗗ���擾����
		std::vector<String> GetIdList(const String& type);

	private:

		AssetManager();
		virtual ~AssetManager() = default;

		template<typename T>
		std::shared_ptr<T> _Load(const String& id)
		{
			return nullptr;
		}

		template<>
		std::shared_ptr<Image> _Load(const String& id)
		{
			return _LoadImage(id);
		}

		/*
		const char* json = R"({
			"type": "sprite",
			"name": "subaru",
			"textureid": "jiki_subaru",
			"atlas": [
				{
					"pos" : [0, 0],
					"size" : [80, 80],
					"pivot" : [0.5, 0.5]
				},
				{
					"matrix": [6, 2],
					"pos" : [0, 80],
					"size" : [80, 80],
					"pivot" : [0.5, 0.5]
				}
			]
		})";
		*/
		template<>
		std::shared_ptr<Sprite> _Load(const String& id)
		{
			return _LoadSprite(id);
		}

		template<>
		std::shared_ptr<BitmapFont> _Load(const String& id)
		{
			return _LoadBitmapFont(id);
		}

		template<>
		std::shared_ptr<FontManager> _Load(const String& id)
		{
			return _LoadFontManager(id);
		}

		template<>
		std::shared_ptr<SE> _Load(const String& id)
		{
			return _LoadSE(id);
		}

		template<>
		std::shared_ptr<BGM> _Load(const String& id)
		{
			return _LoadBGM(id);
		}

		template<>
		std::shared_ptr<Texture> _Load(const String& id)
		{
			return _LoadTexture(id);
		}

		std::shared_ptr<Image> _LoadImage(const String& id);
		std::shared_ptr<Sprite> _LoadSprite(const String& id);
		std::shared_ptr<BitmapFont> _LoadBitmapFont(const String& id);
		std::shared_ptr<FontManager> _LoadFontManager(const String& id);
		std::shared_ptr<BGM> _LoadBGM(const String& id);
		std::shared_ptr<SE> _LoadSE(const String& id);
		std::shared_ptr<Texture> _LoadTexture(const String& id);

		bool m_allowUrlRewrite = true;
		String m_archivePath;
		std::shared_ptr<ArchiveAccessor> m_archiveStream;
		String m_secretKey;
	};
}

#endif
