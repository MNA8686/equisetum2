/**
* @file HMAC.cpp
* @brief HMACによるハッシュ算出クラス
* @author MNA
* @date 2017/07/06 新規作成
*/
#include "crypt/MessageDigest.hpp"
#include "system/Exception.hpp"
#include "system/Logger.h"

#include "crypt/HMAC.hpp"
#include "crypt/MessageDigest.hpp"

namespace Equisetum2
{
	std::shared_ptr<HMAC> HMAC::Create(const String& algorithm, const String& secretKey)
	{
		std::shared_ptr<HMAC> inst;

		EQ_DURING
		{
			class HMACDummy : public HMAC {};
		
			auto inst_ = std::make_shared<HMACDummy>();
			if (!inst_)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			if (!inst_->Init(algorithm, secretKey))
			{
				EQ_THROW(u8"初期化に失敗しました。");
			}

			inst = inst_;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return inst;
	}

	bool HMAC::Init(const String& algorithm, const String& secretKey)
	{
		auto ret = false;

		EQ_DURING
		{
			if (algorithm.empty())
			{
				EQ_THROW(u8"アルゴリズムが設定されていません。");
			}

			if (secretKey.empty())
			{
				EQ_THROW(u8"秘密鍵が設定されていません。");
			}

			if (algorithm == "SHA256")
			{
				m_algorithm = MessageDigest::Create(algorithm);
			}
			else
			{
				EQ_THROW(u8"サポートされていないアルゴリズムです。");
			}

			if (!m_algorithm)
			{
				EQ_THROW(u8"アルゴリズムの作成に失敗しました。");
			}

			// キー生成
			uint8_t key[65] = { 0 };
			if (secretKey.size() > 64)
			{
				if (!m_algorithm->Update((const uint8_t*)secretKey.data(), secretKey.size()))
				{
					EQ_THROW(u8"ハッシュのUpdateに失敗しました。");
				}

				auto digest = m_algorithm->Digest();
				memcpy(key, &digest[0], digest.size());
			}
			else
			{
				memcpy(key, secretKey.data(), secretKey.size());
			}

			// キーを元にipadとopadを生成
			for (int i = 0; i < 64; i++)
			{
				m_ipad[i] = key[i] ^ 0x36;
				m_opad[i] = key[i] ^ 0x5c;
			}

			m_algorithm->Init();

			if (!m_algorithm->Update((const uint8_t*)m_ipad, 64))
			{
				EQ_THROW(u8"ハッシュのUpdateに失敗しました。");
			}

			ret = true;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return ret;
	}

	bool HMAC::Update(const uint8_t* pBuf, size_t size)
	{
		auto ret = false;

		EQ_DURING
		{
			if (!m_algorithm)
			{
				EQ_THROW(u8"初期化されていません。");
			}
		
			if (!pBuf)
			{
				EQ_THROW(u8"入力データが設定されていません。");
			}

			if (!m_algorithm->Update(pBuf, size))
			{
				EQ_THROW(u8"ハッシュのUpdateに失敗しました。");
			}

			ret = true;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return ret;
	}

	std::vector<uint8_t> HMAC::DoFinal()
	{
		std::vector<uint8_t> ret;

		EQ_DURING
		{
			if (!m_algorithm)
			{
				EQ_THROW(u8"初期化されていません。");
			}

			auto digest2 = m_algorithm->Digest();

			m_algorithm->Init();

			if (!m_algorithm->Update((const uint8_t*)m_opad, 64))
			{
				EQ_THROW(u8"ハッシュのUpdateに失敗しました。");
			}

			if (!m_algorithm->Update((const uint8_t*)&digest2[0], digest2.size()))
			{
				EQ_THROW(u8"ハッシュのUpdateに失敗しました。");
			}

			ret = m_algorithm->Digest();
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return ret;
	}
}

