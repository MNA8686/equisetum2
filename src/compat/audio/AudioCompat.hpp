#if !defined(_EQAUDIOCOMPAT_H_)
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
		void DeleteBGM(AudioHandlerID id);
		bool PlayBGM(AudioHandlerID id, bool loop);
		bool StopBGM(AudioHandlerID id);
		void PauseBGM(AudioHandlerID id);
		void ResumeBGM(AudioHandlerID id);
		bool SetVolumeBGM(AudioHandlerID id, double volume);
		double VolumeBGM(AudioHandlerID id) const;
		bool IsPlayingBGM(AudioHandlerID id) const;
		bool IsPausedBGM(AudioHandlerID id) const;
		bool SetPos(AudioHandlerID id, double pos);

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
		bool Replace(AudioHandlerID dstId, AudioHandlerID srcId);

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
