/**
* @file Audio.hpp
* @brief オーディオ統括クラス
* @author MNA
* @date 2017/09/07 新規作成
*/
#if !defined(_EQAUDIO_H_)
#define _EQAUDIO_H_

namespace Equisetum2
{
	class Audio
	{
	public:
		static const double MaxVolume;	/// このライブラリで使用する音のボリューム最大値
		static const double MinVolume;	/// このライブラリで使用する音のボリューム最小値

		/**
		* @brief 全て音を一時停止する
		* @detail 内部で参照カウンタを管理しているため、複数回呼んでも問題ない
		*/
		static void Pause();

		/**
		* @brief 全ての音の再生を再開する
		* @detail 内部で参照カウンタを管理しているため、複数回呼んでも問題ない
		*/
		static void Resume();

		/**
		* @brief 全ての音が一時停止中かどうかを取得する
		* @return 一時中かどうか
		*/
		static bool IsPaused();

		/**
		* @brief マスターボリュームを設定する
		* @param volume ボリューム設定 0～1.0
		* @return 成否
		*/
		static bool SetVolume(double volume);

		/**
		* @brief マスターボリュームを取得する
		* @return ボリューム設定 0～1.0
		*/
		static double Volume();
	};
}

#endif
