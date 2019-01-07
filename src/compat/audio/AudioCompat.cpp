#include "system/Logger.h"
#include "audio/AudioCompat.hpp"

#include "audio/AudioCompatImpl.inl"

namespace Equisetum2
{
	AudioCompat::AudioCompat()
	{
		m_Impl = std::make_shared<AudioCompat::Impl>();
	}

	void AudioCompat::Prepare()
	{
		m_Impl->Prepare();
	}

	void AudioCompat::Update()
	{
		m_Impl->Update();
	}

	void AudioCompat::PauseMaster()
	{
		m_Impl->PauseMaster();
	}

	void AudioCompat::ResumeMaster()
	{
		m_Impl->ResumeMaster();
	}

	bool AudioCompat::IsPausedMaster() const
	{
		return m_Impl->IsPausedMaster();
	}

	bool AudioCompat::SetVolumeMaster(double volume)
	{
		return m_Impl->SetVolumeMaster(volume);
	}

	double AudioCompat::VolumeMaster() const
	{
		return m_Impl->VolumeMaster();
	}

	AudioHandlerID AudioCompat::CreateBGMFromStream(const std::shared_ptr<IStream> stream)
	{
		return m_Impl->CreateBGMFromStream(stream);
	}

	void AudioCompat::DeleteBGM()
	{
		m_Impl->DeleteBGM();
	}

	bool AudioCompat::PlayBGM(bool loop)
	{
		return m_Impl->PlayBGM(loop);
	}

	bool AudioCompat::StopBGM()
	{
		return m_Impl->StopBGM();
	}

	void AudioCompat::PauseBGM()
	{
		m_Impl->PauseBGM();
	}

	void AudioCompat::ResumeBGM()
	{
		m_Impl->ResumeBGM();
	}

	bool AudioCompat::IsPlayingBGM() const
	{
		return m_Impl->IsPlayingBGM();
	}

	bool AudioCompat::IsPausedBGM() const
	{
		return m_Impl->IsPausedBGM();
	}

	bool AudioCompat::SetPos(double pos)
	{
		return m_Impl->SetPos(pos);
	}

	bool AudioCompat::SetVolumeBGM(double volume)
	{
		return m_Impl->SetVolumeBGM(volume);
	}

	double AudioCompat::VolumeBGM() const
	{
		return m_Impl->VolumeBGM();
	}

	AudioHandlerID AudioCompat::CreateSEFromStream(const std::shared_ptr<IStream> stream)
	{
		return m_Impl->CreateSEFromStream(stream);
	}

	void AudioCompat::DeleteSE(AudioHandlerID id)
	{
		m_Impl->DeleteSE(id);
	}

	bool AudioCompat::PlaySE(AudioHandlerID id, bool loop)
	{
		return m_Impl->PlaySE(id, loop);
	}

	bool AudioCompat::StopSE(AudioHandlerID id)
	{
		return m_Impl->StopSE(id);
	}

	void AudioCompat::PauseSE(AudioHandlerID id)
	{
		m_Impl->PauseSE(id);
	}

	void AudioCompat::ResumeSE(AudioHandlerID id)
	{
		m_Impl->ResumeSE(id);
	}

	bool AudioCompat::IsPlayingSE(AudioHandlerID id) const
	{
		return m_Impl->IsPlayingSE(id);
	}

	bool AudioCompat::IsPausedSE(AudioHandlerID id) const
	{
		return m_Impl->IsPausedSE(id);
	}

	bool AudioCompat::SetVolumeSE(AudioHandlerID id, double volume)
	{
		return m_Impl->SetVolumeSE(id, volume);
	}

	double AudioCompat::VolumeSE(AudioHandlerID id) const
	{
		return m_Impl->VolumeSE(id);
	}

	bool AudioCompat::Replace(AudioHandlerID dstId, AudioHandlerID srcId)
	{
		return m_Impl->Replace(dstId, srcId);
	}

	// ボリューム範囲チェック
	double AudioCompat::SatVolume(double volume)
	{
		auto satVolume = volume;

		if (volume > Audio::MaxVolume)
		{
			satVolume = Audio::MaxVolume;
		}
		else if (volume < Audio::MinVolume)
		{
			satVolume = Audio::MinVolume;
		}

		return satVolume;
	}
}

