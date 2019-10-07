#include "system/Exception.hpp"
#include "system/Logger.h"
#include "graphic/BitmapFont.hpp"

namespace Equisetum2
{
	std::shared_ptr<BitmapFont> BitmapFont::CreateFromSprite(const std::shared_ptr<Sprite>& sprite, const String& codePoint)
	{
		EQ_DURING
		{
			if (!sprite)
			{
				EQ_THROW(u8"Spriteが設定されていません。");
			}

			if (sprite->GetAnimAtlas().empty())
			{
				EQ_THROW(u8"SpriteにAtlasが設定されていません。");
			}

			if (sprite->GetAnimAtlas().size() < codePoint.size_by_codepoints())
			{
				EQ_THROW(u8"コードポイント配列のサイズはSpriteに設定されているAtlasサイズ以下でなければいけません。");
			}

			// 高さが全て同じになっているか確認
			const std::vector<stSpriteAnimAtlas>& atlas = sprite->GetAnimAtlas();
			Size srcSize = atlas[0].m_srcSize;
			for (auto& elem : atlas)
			{
				if (srcSize.y != elem.m_srcSize.y)
				{
					EQ_THROW(u8"ビットマップフォントに設定されているパターンは高さが全て同じでなければなりません。");
				}
			}

			// インスタンス作成
			auto inst = std::make_shared<BitmapFont>();
			if (!inst)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			inst->m_codePoint = codePoint.to_u32();
			inst->m_sprite = sprite;
			
			return inst;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return nullptr;
	}

	bool BitmapFont::MoveFrom(std::shared_ptr<BitmapFont>&& src)
	{
		m_id = std::move(src->m_id);
		m_sprite->MoveFrom(std::move(src->m_sprite));
		m_codePoint = std::move(src->m_codePoint);

		return true;
	}

	String Equisetum2::BitmapFont::CodePoint() const
	{
		return String(m_codePoint);
	}

	const std::shared_ptr<Sprite>& BitmapFont::GetSprite() const
	{
		return m_sprite;
	}

	int BitmapFont::CodePointToAtlas(char32_t code)
	{
		int atlas = -1;
		int count = 0;
		for (auto& c : m_codePoint)
		{
			if (c == code)
			{
				atlas = count;
				break;
			}

			count++;
		}

		return atlas;
	}

	void BitmapFont::SetIdentify(const String& id)
	{
		m_id = id;
	}

	String BitmapFont::Identify() const
	{
		return m_id;
	}
}
