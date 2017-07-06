/**
* @file MessageDigest.cpp
* @brief MessageDigestによるハッシュ算出クラス
* @author MNA
* @date 2017/05/26 新規作成
*/
#include "crypt/MessageDigest.hpp"
#include "system/Exception.hpp"
#include "system/Logger.h"

#include "crypt/MessageDigestSHA256.hpp"

namespace Equisetum2
{
	std::shared_ptr<IMessageDigest> MessageDigest::Create(const String& algorithm)
	{
		std::shared_ptr<IMessageDigest> inst;

		EQ_DURING
		{
			if (algorithm == "SHA256")
			{
				inst = SHA256::MessageDigest::Create();
			}
			else
			{
				EQ_THROW(u8"サポートされていないアルゴリズムです。");
			}
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return inst;
	}
}
