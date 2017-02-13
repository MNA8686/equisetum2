#if !defined(_EQJOYSTICK_H_)
#define _EQJOYSTICK_H_

#include "input/IKeyState.h"
#include "input/Key.h"
#include <memory>
#include <functional>

namespace Equisetum2
{
	// ジョイスティックのアクセスクラス
	// 存在しない番号にアクセスしてもクラッシュしないことが保証されている
	// 挿抜があってもインスタンスを作り直す必要はない

	typedef struct
	{
		std::function<void()> OnConnected;
		std::function<void()> OnRemoved;
	}stJoystickEvent;

	class Joystick
	{
	public:

		static std::shared_ptr<Joystick> CreateByIndex(int index);

		bool IsConnected() const;
		int16_t Axis(int num) const;
		const Key HatUp(int num) const;
		const Key HatDown(int num) const;
		const Key HatLeft(int num) const;
		const Key HatRight(int num) const;
		const Key Button(int num) const;

		int NumAxes() const;
		int NumButtons() const;
		int NumHats() const;
		const std::string Name() const;
		int Index();
		void SetEventListener(const stJoystickEvent& listener);

		static int NumJoysticks();

	protected:

		Joystick(int index);	// インスタンス作成封じ

	private:

		int m_Index = 0;
	};
}

#endif
