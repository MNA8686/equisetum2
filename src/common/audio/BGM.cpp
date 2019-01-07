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
					Singleton<AudioCompat>::GetInstance()->DeleteBGM();
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
		return Singleton<AudioCompat>::GetInstance()->PlayBGM(loop);
	}

	bool BGM::Stop()
	{
		return Singleton<AudioCompat>::GetInstance()->StopBGM();
	}

	void BGM::Pause()
	{
		Singleton<AudioCompat>::GetInstance()->PauseBGM();
	}

	void BGM::Resume()
	{
		Singleton<AudioCompat>::GetInstance()->ResumeBGM();
	}

	bool BGM::IsPlaying() const
	{
		return Singleton<AudioCompat>::GetInstance()->IsPlayingBGM();
	}

	bool BGM::IsPaused() const
	{
		return Singleton<AudioCompat>::GetInstance()->IsPausedBGM();
	}

	bool BGM::SetVolume(double volume)
	{
		return Singleton<AudioCompat>::GetInstance()->SetVolumeBGM(volume);
	}

	double BGM::Volume() const
	{
		return Singleton<AudioCompat>::GetInstance()->VolumeBGM();
	}

	bool BGM::SetPos(double pos)
	{
		return Singleton<AudioCompat>::GetInstance()->SetPos(pos);
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

