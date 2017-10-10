/**
* @file SE.cpp
* @brief SE再生クラス
* @author MNA
* @date 2017/09/07 新規作成
*/
#include "audio/SE.hpp"
#include "system/Logger.h"
#include "system/Exception.hpp"
#include "audio/AudioCompat.hpp"

namespace Equisetum2
{
	std::shared_ptr<SE> SE::CreateFromStream(const std::shared_ptr<IStream> stream)
	{
		std::shared_ptr<SE> inst;

		EQ_DURING
		{
			auto seHandler = Singleton<AudioCompat>::GetInstance()->CreateSEFromStream(stream);
			if (seHandler < 0)
			{
				EQ_THROW(u8"SEの作成に失敗しました。");
			}

			// デリーター作成
			auto id = std::shared_ptr<uint32_t>(new uint32_t,
				[](uint32_t* pId) {
				if (pId != nullptr)
				{
					Singleton<AudioCompat>::GetInstance()->DeleteSE(*pId);
					delete pId;
				}
			});

			if (!id)
			{
				EQ_THROW(u8"デリーターの作成に失敗しました。");
			}

			*id = seHandler;

			auto inst_ = std::make_shared<SE>();
			if (!inst_)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			inst_->m_handle = id;
			inst = inst_;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER
			
		return inst;
	}

	bool SE::Play(bool loop)
	{
		return Singleton<AudioCompat>::GetInstance()->PlaySE(*m_handle, loop);
	}

	bool SE::Stop()
	{
		return Singleton<AudioCompat>::GetInstance()->StopSE(*m_handle);
	}

	void SE::Pause()
	{
		Singleton<AudioCompat>::GetInstance()->PauseSE(*m_handle);
	}

	void SE::Resume()
	{
		Singleton<AudioCompat>::GetInstance()->ResumeSE(*m_handle);
	}

	bool SE::IsPlaying() const
	{
		return Singleton<AudioCompat>::GetInstance()->IsPlayingSE(*m_handle);
	}

	bool SE::IsPaused() const
	{
		return Singleton<AudioCompat>::GetInstance()->IsPausedSE(*m_handle);
	}

	bool SE::SetVolume(double volume)
	{
		return Singleton<AudioCompat>::GetInstance()->SetVolumeSE(*m_handle, volume);
	}

	double SE::Volume() const
	{
		return Singleton<AudioCompat>::GetInstance()->VolumeSE(*m_handle);
	}

	bool SE::Replace(std::shared_ptr<SE> src)
	{
		auto ret = false;
		
		if(*m_handle != *(src->m_handle))
		{
			ret = Singleton<AudioCompat>::GetInstance()->Replace(*m_handle, *(src->m_handle));
		}
		
		return ret;
	}

	void SE::SetIdentify(const String& id)
	{
		m_id = id;
	}

	String SE::Identify() const
	{
		return m_id;
	}
}

