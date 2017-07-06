/**
* @file SHA256.cpp
* @brief SHA256によるハッシュ算出、及びHMAC算出クラス
* @author MNA
* @date 2017/05/18 新規作成
*/
#include "util/SHA256.hpp"
#include "crypt/HMAC.hpp"
#include "crypt/MessageDigest.hpp"
#include "system/Exception.hpp"
#include "system/Logger.h"
#include "stream/MemoryStream.h"
#include <stdio.h>

#include <array>

namespace Equisetum2
{
	bool SHA256Value::operator == (const SHA256Value& other) const
	{
		auto ret = true;

		for (auto i = 0; i < sizeof(value) / sizeof(value[0]); i++)
		{
			if (value[i] != other.value[i])
			{
				ret = false;
				break;
			}
		}

		return ret;
	}

	bool SHA256Value::operator != (const SHA256Value& other) const
	{
		return !(*this == other);
	}

	String SHA256Value::ToString() const
	{
		String out;

		for (auto val : value)
		{
			out += String::Sprintf("%02X", val);
		}

		return out;
	}

	Optional<SHA256Value> SHA256::Encrypt(std::shared_ptr<IStream> stream)
	{
		Optional<SHA256Value> ret;

		EQ_DURING
		{
			if (!stream)
			{
				EQ_THROW(u8"ストリームが設定されていません。");
			}

			if (!stream->CanRead())
			{
				EQ_THROW(u8"ストリームに読み出し属性が必要です。");
			}

			auto sha256 = MessageDigest::Create("SHA256");
			if (!sha256)
			{
				EQ_THROW(u8"MessageDigestの作成に失敗しました。");
			}

			std::vector<uint8_t> buf(4096);

			while (1)
			{
				auto readResult = stream->Read(buf, buf.size());
				if (!readResult)
				{
					EQ_THROW(u8"ストリームの読み出しに失敗しました。");
				}

				if (!sha256->Update((const uint8_t*)&buf[0], *readResult))
				{
					EQ_THROW(u8"ハッシュのUpdateに失敗しました。");
				}

				if (*readResult == 0)
				{
					break;
				}
			}

			auto digest = sha256->Digest();

			SHA256Value val;
			memcpy(val.value, &digest[0], digest.size());

			ret = val;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return ret;
	}

	Optional<SHA256Value> SHA256::HMAC(std::shared_ptr<IStream> stream, const String& stringKey)
	{
		Optional<SHA256Value> ret;

		EQ_DURING
		{
			if (!stream)
			{
				EQ_THROW(u8"ストリームが設定されていません。");
			}

			if (!stream->CanRead())
			{
				EQ_THROW(u8"ストリームに読み出し属性が必要です。");
			}

			if (stringKey.size() == 0)
			{
				EQ_THROW(u8"キーが設定されていません。");
			}

			auto inst = HMAC::Create("SHA256", stringKey);
			if(!inst)
			{
				EQ_THROW(u8"HMACインスタンスの作成に失敗しました。");
			}

			std::vector<uint8_t> buf(4096);
			while (1)
			{
				auto readResult = stream->Read(buf, buf.size());
				if (!readResult)
				{
					EQ_THROW(u8"ストリームの読み出しに失敗しました。");
				}

				if (!inst->Update((const uint8_t*)&buf[0], *readResult))
				{
					EQ_THROW(u8"ハッシュのUpdateに失敗しました。");
				}

				if (*readResult == 0)
				{
					break;
				}
			}

			auto digest3 = inst->DoFinal();

			SHA256Value val;
			memcpy(val.value, &digest3[0], digest3.size());

			ret = val;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return ret;
	}
}

