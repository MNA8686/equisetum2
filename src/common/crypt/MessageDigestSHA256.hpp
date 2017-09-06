/**
* @file MessageDigestSHA256.hpp
* @brief MessageDigestSHA256によるハッシュ算出クラス
* @author MNA
* @date 2017/05/26 新規作成
*/
#if !defined(_EQMESSAGEDIGESTSHA256_H_)
#define _EQMESSAGEDIGESTSHA256_H_

#include "crypt/MessageDigest.hpp"
#include "stream/MemoryStream.h"
#include <array>

namespace Equisetum2
{
	namespace SHA256
	{
		// 型定義
		typedef uint32_t SHA_INT_TYPE;

		//SHA256のメッセージダイジェスト初期値
		static const SHA_INT_TYPE SHA256_H_Val[] = { 0x6a09e667 , 0xbb67ae85 , 0x3c6ef372 , 0xa54ff53a , 0x510e527f , 0x9b05688c , 0x1f83d9ab , 0x5be0cd19 };

		//SHA256の加算定数
		static const SHA_INT_TYPE SHA256_K_Val[] = { 0x428a2f98 , 0x71374491 , 0xb5c0fbcf , 0xe9b5dba5 , 0x3956c25b , 0x59f111f1 , 0x923f82a4 , 0xab1c5ed5 ,
			0xd807aa98 , 0x12835b01 , 0x243185be , 0x550c7dc3 , 0x72be5d74 , 0x80deb1fe , 0x9bdc06a7 , 0xc19bf174 ,
			0xe49b69c1 , 0xefbe4786 , 0x0fc19dc6 , 0x240ca1cc , 0x2de92c6f , 0x4a7484aa , 0x5cb0a9dc , 0x76f988da ,
			0x983e5152 , 0xa831c66d , 0xb00327c8 , 0xbf597fc7 , 0xc6e00bf3 , 0xd5a79147 , 0x06ca6351 , 0x14292967 ,
			0x27b70a85 , 0x2e1b2138 , 0x4d2c6dfc , 0x53380d13 , 0x650a7354 , 0x766a0abb , 0x81c2c92e , 0x92722c85 ,
			0xa2bfe8a1 , 0xa81a664b , 0xc24b8b70 , 0xc76c51a3 , 0xd192e819 , 0xd6990624 , 0xf40e3585 , 0x106aa070 ,
			0x19a4c116 , 0x1e376c08 , 0x2748774c , 0x34b0bcb5 , 0x391c0cb3 , 0x4ed8aa4a , 0x5b9cca4f , 0x682e6ff3 ,
			0x748f82ee , 0x78a5636f , 0x84c87814 , 0x8cc70208 , 0x90befffa , 0xa4506ceb , 0xbef9a3f7 , 0xc67178f2 };

		//プロトタイプ宣言(基本関数)
		static inline SHA_INT_TYPE SHA256_rotr(SHA_INT_TYPE, SHA_INT_TYPE);
		static inline SHA_INT_TYPE SHA256_sigma0(SHA_INT_TYPE);
		static inline SHA_INT_TYPE SHA256_sigma1(SHA_INT_TYPE);
		static inline SHA_INT_TYPE SHA256_sum0(SHA_INT_TYPE);
		static inline SHA_INT_TYPE SHA256_sum1(SHA_INT_TYPE);
		static inline SHA_INT_TYPE SHA256_ch(SHA_INT_TYPE, SHA_INT_TYPE, SHA_INT_TYPE);
		static inline SHA_INT_TYPE SHA256_maj(SHA_INT_TYPE, SHA_INT_TYPE, SHA_INT_TYPE);

		static inline void SHA_Reverse_INT64(const unsigned char *, unsigned __int64);
		static inline SHA_INT_TYPE SHA_Reverse(SHA_INT_TYPE);

		//プロトタイプ宣言(SHA)
		static inline void SHA256_HashBlock(SHA_INT_TYPE *, const unsigned char *);

		//右ローテート関数
		static inline SHA_INT_TYPE SHA256_rotr(SHA_INT_TYPE r, SHA_INT_TYPE x)
		{
			SHA_INT_TYPE rot = r % 32;
			return (x >> rot) | (x << (32 - rot));
		}

		static inline SHA_INT_TYPE SHA256_sigma0(SHA_INT_TYPE x)
		{
			return SHA256_rotr(7, x) ^ SHA256_rotr(18, x) ^ (x >> 3);
		}

		static inline SHA_INT_TYPE SHA256_sigma1(SHA_INT_TYPE x)
		{
			return SHA256_rotr(17, x) ^ SHA256_rotr(19, x) ^ (x >> 10);
		}

		static inline SHA_INT_TYPE SHA256_sum0(SHA_INT_TYPE x)
		{
			return SHA256_rotr(2, x) ^ SHA256_rotr(13, x) ^ SHA256_rotr(22, x);
		}

		static inline SHA_INT_TYPE SHA256_sum1(SHA_INT_TYPE x)
		{
			return SHA256_rotr(6, x) ^ SHA256_rotr(11, x) ^ SHA256_rotr(25, x);
		}

		static inline SHA_INT_TYPE SHA256_ch(SHA_INT_TYPE x, SHA_INT_TYPE y, SHA_INT_TYPE z)
		{
			return (x & y) ^ (~x & z);
		}

		static inline SHA_INT_TYPE SHA256_maj(SHA_INT_TYPE x, SHA_INT_TYPE y, SHA_INT_TYPE z)
		{
			return (x&y) ^ (x&z) ^ (y&z);
		}

		static inline void SHA_Reverse_INT64(unsigned char *data,  uint64_t write)
		{
			unsigned char cdata[8];
			memcpy(cdata, &write, sizeof(uint64_t));
			for (int i = 0; i <= 7; i++)
			{
				*(data + i) = cdata[7 - i];
			}
		}

		static inline SHA_INT_TYPE SHA_Reverse(SHA_INT_TYPE d)
		{
			unsigned char b_data[4], a_data[4];
			SHA_INT_TYPE ret;
			memcpy(b_data, &d, sizeof(uint32_t));
			for (int i = 0; i < 4; i++)
			{
				a_data[i] = b_data[3 - i];
			}
			memcpy(&ret, a_data, sizeof(a_data));
			return ret;
		}


		static inline void SHA256_HashBlock(SHA_INT_TYPE *SHA256_H_Data, const unsigned char *data)
		{
			SHA_INT_TYPE SIT[64];
			SHA_INT_TYPE SIT_d[16];//512ビット、64バイト
			SHA_INT_TYPE a, b, c, d, e, f, g, h;
			for (int i = 0, j = 0; i < 16; i++, j += 4)
			{
				SIT_d[i] = ((*(data + j + 3) & 0xFF) << 24) | ((*(data + j + 2) & 0xFF) << 16) | ((*(data + j + 1) & 0xFF) << 8) | ((*(data + j) & 0xFF));
			}
			for (int i = 0; i < 16; i++)
			{
				SIT[i] = SHA_Reverse(SIT_d[i]);
			}
			for (int t = 16; t <= 63; t++)
			{
				SIT[t] = SHA256_sigma1(SIT[t - 2]) + SIT[t - 7] + SHA256_sigma0(SIT[t - 15]) + SIT[t - 16];
			}
			a = *SHA256_H_Data;
			b = *(SHA256_H_Data + 1);
			c = *(SHA256_H_Data + 2);
			d = *(SHA256_H_Data + 3);
			e = *(SHA256_H_Data + 4);
			f = *(SHA256_H_Data + 5);
			g = *(SHA256_H_Data + 6);
			h = *(SHA256_H_Data + 7);
			for (int t = 0; t <= 63; t++)
			{
				SHA_INT_TYPE tmp[2];
				tmp[0] = h + SHA256_sum1(e) + SHA256_ch(e, f, g) + SHA256_K_Val[t] + SIT[t];
				tmp[1] = SHA256_sum0(a) + SHA256_maj(a, b, c);
				h = g;
				g = f;
				f = e;
				e = d + tmp[0];
				d = c;
				c = b;
				b = a;
				a = tmp[0] + tmp[1];
			}
			*SHA256_H_Data += a;
			*(SHA256_H_Data + 1) += b;
			*(SHA256_H_Data + 2) += c;
			*(SHA256_H_Data + 3) += d;
			*(SHA256_H_Data + 4) += e;
			*(SHA256_H_Data + 5) += f;
			*(SHA256_H_Data + 6) += g;
			*(SHA256_H_Data + 7) += h;
		}

		class MessageDigest : public IMessageDigest
		{
		public:

			static std::shared_ptr<IMessageDigest> Create()
			{
				std::shared_ptr<IMessageDigest> ret;

				auto ptr = std::make_shared<MessageDigest>();
				if (ptr && ptr->Init())
				{
					ret = ptr;
				}

				return ret;
			}

			MessageDigest(){}

			virtual ~MessageDigest() {}

			virtual bool Init() override
			{
				auto ret = true;

				memcpy(m_digest, SHA256_H_Val, sizeof(SHA256_H_Val));
				m_stream = MemoryStream::CreateFromBuf(&m_buf[0], (int)m_buf.size());
				dataSize = 0;

				if (!m_stream)
				{
					ret = false;
				}

				return ret;
			}

			virtual bool Update(const uint8_t* pBuf, size_t size) override
			{
				size_t remain = size;
				auto ret = true;

				// 入力データを全て処理するまでループ
				while (remain > 0)
				{
					// バッファ埋め
					auto copiedSize = m_stream->Write(pBuf, remain);
					if (!copiedSize || *copiedSize == 0)
					{
						ret = false;
						break;
					}

					// バッファフル？
					if (m_stream->Position() == m_buf.size())
					{
						// バッファを処理する
						SHA256_HashBlock(m_digest, &m_buf[0]);
						dataSize += m_buf.size();

						// バッファクリア
						m_stream->Seek(0, SeekOrigin::Begin);
					}

					pBuf += *copiedSize;
					remain -= *copiedSize;
				}

				return ret;
			}

			virtual std::vector<uint8_t> Digest() override
			{
				dataSize += (size_t)m_stream->Position();

				// 処理するデータのサイズ
				SHA_INT_TYPE mod = dataSize % (int)m_buf.size();

				// パディングを設定する
				for (auto i = mod; i < m_buf.size(); i++)
				{
					m_buf[i] = 0;
				}

				// 0x80を設定する
				m_buf[mod] = 0x80;

				// パディングに含めるデータのサイズが56バイト以上だった時の処理
				if (mod >= 56)
				{
					// パディングに含めるデータのサイズが56バイト以上だったらSHA1_HashBlockを実行する
					SHA256_HashBlock(m_digest, &m_buf[0]);

					// 最初～56バイト分NULL文字をセット
					memset(&m_buf[0], 0, 56);
				}

				// データのサイズをビット数にする
				uint64_t s64 = dataSize * 8;

				// データのサイズ(ビット単位)を書き込む
				SHA_Reverse_INT64(&m_buf[56], s64);

				// 最後の処理
				SHA256_HashBlock(m_digest, &m_buf[0]);

				std::vector<uint8_t> digest(32);
				for (size_t i = 0; i < digest.size(); i++)
				{
					digest[i] = (m_digest[i / 4] >> (24 - (8 * (i % 4)))) & 0xff;
				}

				return digest;
			}

		private:

			std::array<uint8_t, 64> m_buf;		// SHA256で使用するバッファ(64バイト単位)
			std::shared_ptr<IStream> m_stream;	// 上記バッファをストリームとしてアクセスするためのインスタンス

			SHA_INT_TYPE m_digest[8];	// メッセージダイジェストの結果
			size_t dataSize = 0;		// 入力されたデータサイズの合計
		};
	}
}
#endif
