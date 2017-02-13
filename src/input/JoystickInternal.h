#if !defined(_EQJOYSTICKINTERNAL_H_)
#define _EQJOYSTICKINTERNAL_H_

#include "system/Singleton.h"
#include "input/IKeyState.h"
#include "input/Joystick.h"
#include <memory>

namespace Equisetum2
{
	// ジョイスティックのポーティングクラス
	// 存在しないジョイスティックの番号を入力してもクラッシュしないことが保証されている

	class JoystickInternal final
	{
	public:
		friend Singleton<JoystickInternal>;	// シングルトンからインスタンスを作成してもらえるようにする

		void Prepare();
		void Update();

		bool IsConnected(int joyIndex) const;
		int16_t Axis(int joyIndex, int num) const;
		HatState Hat(int joyIndex, int num) const;
		const stKeyState& Button(int joyIndex, int num) const;

		int NumAxes(int joyIndex) const;
		int NumButtons(int joyIndex) const;
		int NumHats(int joyIndex) const;
		const std::string Name(int joyIndex) const;
		int NumJoysticks() const;

	private:

		class Impl;
		std::shared_ptr<Impl> m_Impl;
		JoystickInternal();	// インスタンス作成封じ
	};
}

#endif
