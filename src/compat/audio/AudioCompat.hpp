﻿#if !defined(_EQAUDIOCOMPAT_H_)
#define _EQAUDIOCOMPAT_H_

#include "system/Singleton.h"
#include "stream/IStream.h"
#include <memory>

namespace Equisetum2
{
	// オーディオのポーティングクラス
	using AudioHandlerID = int32_t;

	class AudioCompat final
	{
	public:
		friend Singleton<AudioCompat>;	// シングルトンからインスタンスを作成してもらえるようにする

		// マスター関連メソッド
		void PauseMaster();
		void ResumeMaster();
		bool IsPausedMaster() const;
		bool SetVolumeMaster(double volume);
		double VolumeMaster() const;

		// BGM関連メソッド
		AudioHandlerID CreateBGMFromStream(const std::shared_ptr<IStream> stream);
		void DeleteBGM();
		bool PlayBGM(bool loop);
		bool StopBGM();
		void PauseBGM();
		void ResumeBGM();
		bool SetVolumeBGM(double volume);
		double VolumeBGM() const;
		bool IsPlayingBGM() const;
		bool IsPausedBGM() const;

		// SE関連メソッド
		AudioHandlerID CreateSEFromStream(const std::shared_ptr<IStream> stream);
		void DeleteSE(AudioHandlerID id);
		bool PlaySE(AudioHandlerID id, bool loop);
		bool StopSE(AudioHandlerID id);
		void PauseSE(AudioHandlerID id);
		void ResumeSE(AudioHandlerID id);
		bool SetVolumeSE(AudioHandlerID id, double volume);
		double VolumeSE(AudioHandlerID id) const;
		bool IsPlayingSE(AudioHandlerID id) const;
		bool IsPausedSE(AudioHandlerID id) const;

		// ボリューム範囲チェック
		static double SatVolume(double volume);

		void Prepare();
		void Update();

	private:

		class Impl;
		std::shared_ptr<Impl> m_Impl;
		AudioCompat();	// インスタンス作成封じ
	};
}

#endif
