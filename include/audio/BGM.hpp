/**
* @file BGM.hpp
* @brief BGM再生クラス
* @author MNA
* @date 2017/09/07 新規作成
*/
#if !defined(_EQBGM_H_)
#define _EQBGM_H_

#include "audio/ISound.hpp"
#include "stream/IStream.h"
#include <memory>

namespace Equisetum2
{
	/**
	* BGMを再生するためのクラス。
	*/
	class BGM : public ISound
	{
	public:
		/**
		* @brief ストリームからBGMをストリーミング再生するインスタンスを作成する
		* @param stream 入力ストリーム(読み出し、シーク属性が必要)
		* @return 作成したインスタンス
		*/
		static std::shared_ptr<BGM> CreateFromStream(const std::shared_ptr<IStream> stream);

		bool Play(bool loop) override;
		bool Stop() override;
		void Pause() override;
		void Resume() override;
		bool IsPlaying() const override;
		bool IsPaused() const override;
		bool SetVolume(double volume) override;
		double Volume() const override;
		bool SetPos(double pos);

		/**
		* @brief イメージのIDを設定する
		*/
		virtual void SetIdentify(const String& id);

		/**
		* @brief イメージのIDを取得する
		* @return イメージのID
		*/
		virtual String Identify() const;

	private:
		std::shared_ptr<uint32_t> m_id;
		String m_identify;
	};
}

#endif
