/**
* @file Audio.cpp
* @brief オーディオ統括クラス
* @author MNA
* @date 2017/09/07 新規作成
*/
#include "audio/Audio.hpp"
#include "system/Logger.h"
#include "system/Exception.hpp"
#include "audio/AudioCompat.hpp"

namespace Equisetum2
{
	const double Audio::MaxVolume = 1.0;
	const double Audio::MinVolume = 0;

	void Audio::Pause()
	{
		Singleton<AudioCompat>::GetInstance()->PauseMaster();
	}

	void Audio::Resume()
	{
		Singleton<AudioCompat>::GetInstance()->ResumeMaster();
	}

	bool Audio::IsPaused()
	{
		return Singleton<AudioCompat>::GetInstance()->IsPausedMaster();
	}

	bool Audio::SetVolume(double volume)
	{
		return Singleton<AudioCompat>::GetInstance()->SetVolumeMaster(volume);
	}

	double Audio::Volume()
	{
		return Singleton<AudioCompat>::GetInstance()->VolumeMaster();
	}
}

