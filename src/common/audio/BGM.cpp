/**
* @file BGM.cpp
* @brief BGM再生クラス
* @author MNA
* @date 2017/09/07 新規作成
*/
#include "audio/BGM.hpp"
#include "system/Logger.h"
#include "system/Exception.hpp"
#include "audio/AudioCompat.hpp"

namespace Equisetum2
{
	std::shared_ptr<BGM> BGM::CreateFromStream(const std::shared_ptr<IStream> stream)
	{
		std::shared_ptr<BGM> inst;

		EQ_DURING
		{
			auto handler = Singleton<AudioCompat>::GetInstance()->CreateBGMFromStream(stream);
			if (handler < 0)
			{
				EQ_THROW(u8"BGMの作成に失敗しました。");
			}

			// デリーター作成
			auto id = std::shared_ptr<uint32_t>(new uint32_t,
				[](uint32_t* pId) {
				if (pId != nullptr)
				{
					Singleton<AudioCompat>::GetInstance()->DeleteBGM(*pId);
					delete pId;
				}
			});

			if (!id)
			{
				EQ_THROW(u8"デリーターの作成に失敗しました。");
			}

			*id = handler;

			auto inst_ = std::make_shared<BGM>();
			if (!inst_)
			{
				EQ_THROW(u8"インスタンスの作成に失敗しました。");
			}

			inst_->m_id = id;
			inst = inst_;
		}
		EQ_HANDLER
		{
			Logger::OutputError(EQ_GET_HANDLER().what());
		}
		EQ_END_HANDLER

		return inst;
	}

	bool BGM::Play(bool loop)
	{
		return Singleton<AudioCompat>::GetInstance()->PlayBGM(*m_id, loop);
	}

	bool BGM::Stop()
	{
		return Singleton<AudioCompat>::GetInstance()->StopBGM(*m_id);
	}

	void BGM::Pause()
	{
		Singleton<AudioCompat>::GetInstance()->PauseBGM(*m_id);
	}

	void BGM::Resume()
	{
		Singleton<AudioCompat>::GetInstance()->ResumeBGM(*m_id);
	}

	bool BGM::IsPlaying() const
	{
		return Singleton<AudioCompat>::GetInstance()->IsPlayingBGM(*m_id);
	}

	bool BGM::IsPaused() const
	{
		return Singleton<AudioCompat>::GetInstance()->IsPausedBGM(*m_id);
	}

	bool BGM::SetVolume(double volume)
	{
		return Singleton<AudioCompat>::GetInstance()->SetVolumeBGM(*m_id, volume);
	}

	double BGM::Volume() const
	{
		return Singleton<AudioCompat>::GetInstance()->VolumeBGM(*m_id);
	}

	bool BGM::SetPos(double pos)
	{
		return Singleton<AudioCompat>::GetInstance()->SetPos(*m_id, pos);
	}

	void BGM::SetIdentify(const String& id)
	{
		m_identify = id;
	}

	String BGM::Identify() const
	{
		return m_identify;
	}
}

