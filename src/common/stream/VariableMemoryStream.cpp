#include "stream/VariableMemoryStream.h"
#include "system/Exception.hpp"
#include "system/Logger.h"

//#include "stream/MemoryStream.inl"

namespace Equisetum2
{
	// どこに置こう
	static bool CopyTo(IStream* pSrc, IStream* pDst)
	{
		auto result = false;

		if (pSrc &&
			pDst &&
			pSrc->CanRead() &&
			pDst->CanWrite())
		{
			std::vector<uint8_t> buf(40960);

			while (auto readSize = pSrc->Read(&buf[0], buf.size()))
			{
				if (*readSize == 0)
				{
					result = true;
					break;
				}

				auto writeSize = pDst->Write(&buf[0], *readSize);
				if (!writeSize || *writeSize < *readSize)
				{
					break;
				}
			}
		}

		return result;
	}

	std::shared_ptr<VariableMemoryStream> VariableMemoryStream::Create(int64_t size)
	{
		std::shared_ptr<VariableMemoryStream> inst;

		EQ_DURING
		{
			auto inst_ = std::shared_ptr<VariableMemoryStream>(new VariableMemoryStream);
			if (!inst_)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			inst_->m_url = String::Sprintf(u8"variablememory://%lld", size);

			if (size > 0)
			{
				inst_->m_buf.resize(static_cast<size_t>(size));

				for (auto& e : inst_->m_buf)
				{
					e = 0;
				}
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

	bool VariableMemoryStream::CanRead() const
	{
		// 固定
		return true;
	}

	bool VariableMemoryStream::CanSeek() const
	{
		// 固定
		return true;
	}

	bool VariableMemoryStream::CanWrite() const
	{
		// 固定
		return true;
	}

	int64_t VariableMemoryStream::Position() const
	{
		return m_pos;
	}

	int64_t VariableMemoryStream::Length() const
	{
		return static_cast<int64_t>(m_buf.size());
	}

	bool VariableMemoryStream::CopyTo(std::shared_ptr<IStream> stream)
	{
		return Equisetum2::CopyTo(this, stream ? stream.get() : nullptr);
	}

	int64_t VariableMemoryStream::Seek(int64_t offset, SeekOrigin origin)
	{
		if (origin == SeekOrigin::Begin)
		{
			if (offset < 0)
			{
				m_pos = 0;
			}
			else if (offset > Length())
			{
				m_pos = Length();
			}
			else
			{
				m_pos = offset;
			}
		}
		else if (origin == SeekOrigin::Current)
		{
			if (m_pos + offset > Length())
			{
				m_pos = Length();
			}
			else if (m_pos + offset < 0)
			{
				m_pos = 0;
			}
			else
			{
				m_pos += offset;
			}
		}
		else if (origin == SeekOrigin::End)
		{
			if (offset > 0)
			{
				m_pos = Length();
			}
			else if (offset < -Length())
			{
				m_pos = 0;
			}
			else
			{
				m_pos = Length() + offset;
			}
		}

		return m_pos;
	}

	const Optional<size_t> VariableMemoryStream::Read(uint8_t *data, size_t size)
	{
		Optional<size_t> opt;

		if (data)
		{
			size_t readSize = size;

			if (static_cast<size_t>(m_pos) + size > m_buf.size())
			{
				readSize = m_buf.size() - static_cast<size_t>(m_pos);
			}

			if (readSize > 0)
			{
				::memcpy(data, &m_buf[static_cast<size_t>(m_pos)], readSize);
			}
			m_pos += readSize;

			opt = readSize;
		}

		return opt;
	}

	const Optional<size_t> VariableMemoryStream::Write(const uint8_t *data, size_t size)
	{
		Optional<size_t> opt;

		if (data)
		{
			auto newSize = static_cast<size_t>(m_pos) + size;

			if (newSize > m_buf.size())
			{
				m_buf.resize(newSize);
			}

			if (size > 0)
			{
				::memcpy(&m_buf[static_cast<size_t>(m_pos)], data, size);
			}

			m_pos += size;

			opt = size;
		}

		return opt;
	}

	int VariableMemoryStream::ReadByte()
	{
		int readData = -1;

		if (m_pos != m_buf.size())
		{
			readData = m_buf[static_cast<size_t>(m_pos)];
			m_pos++;
		}

		return readData;
	}

	int VariableMemoryStream::WriteByte(uint8_t writeData)
	{
		auto opt = Write(&writeData, 1);

		if (opt && *opt == 1)
		{
			return 1;
		}

		return -1;
	}

	String VariableMemoryStream::Url()
	{
		return m_url;
	}

	const void* VariableMemoryStream::Data() const
	{
		return m_buf.data();
	}
}
