/**
* @file SE.hpp
* @brief SE再生クラス
* @author MNA
* @date 2017/09/07 新規作成
*/
#if !defined(_EQSE_H_)
#define _EQSE_H_

#include "audio/ISound.hpp"
#include "stream/IStream.h"
#include <memory>

namespace Equisetum2
{
	/**
	* SEを再生するためのクラス。
	*/
	class SE : public ISound
	{
	public:
		/**
		* @brief ストリームからSEを作成し、再生するインスタンスを作成する
		* @param stream 入力ストリーム(読み出し、シーク属性が必要)
		* @return 作成したインスタンス
		*/
		static std::shared_ptr<SE> CreateFromStream(const std::shared_ptr<IStream> stream);

		bool Play(bool loop) override;
		bool Stop() override;
		void Pause() override;
		void Resume() override;
		bool IsPlaying() const override;
		bool IsPaused() const override;
		bool SetVolume(double volume) override;
		double Volume() const override;

		/**
		* @brief 新しいインスタンスで現在のインスタンスを置き換える
		* @param src 置き換え元のインスタンス
		* @return 成否
		*/
		bool Replace(std::shared_ptr<SE> src);

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
		std::shared_ptr<uint32_t> m_handle;

		String m_id;
	};
}

#endif
