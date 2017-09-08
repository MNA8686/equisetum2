/**
* @file ISound.hpp
* @brief 音操作の抽象クラス
* @author MNA
* @date 2017/09/07 新規作成
*/
#if !defined(_EQISOUND_H_)
#define _EQISOUND_H_

namespace Equisetum2
{
	/**
	* 音操作の抽象クラス。<br>
	* 音を扱うクラスはこれを派生して使う。
	*/
	class ISound
	{
	public:
		/**
		* @brief 音を再生する
		* @param loop ループさせたいときに設定する
		* @return 成否
		*/
		virtual bool Play(bool loop) = 0;

		/**
		* @brief 音を停止する
		* @return 成否
		*/
		virtual bool Stop() = 0;

		/**
		* @brief 音を一時停止する
		* @detail 内部で参照カウンタを管理しているため、複数回呼んでも問題ない
		*/
		virtual void Pause() = 0;

		/**
		* @brief 音の再生を再開する
		* @detail 内部で参照カウンタを管理しているため、複数回呼んでも問題ない
		*/
		virtual void Resume() = 0;

		/**
		* @brief 音が再生中かどうかを取得する
		* @return 再生中かどうか
		*/
		virtual bool IsPlaying() const = 0;

		/**
		* @brief 音が一時停止中かどうかを取得する
		* @return 一時停止中かどうか
		*/
		virtual bool IsPaused() const = 0;

		/**
		* @brief 音のボリュームを設定する
		* @param volume ボリューム設定 0～1.0
		* @return 成否
		*/
		virtual bool SetVolume(double volume) = 0;

		/**
		* @brief 音のボリュームを取得する
		* @return ボリューム設定 0～1.0
		*/
		virtual double Volume() const = 0;
	};
}

#endif
