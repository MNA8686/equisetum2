#include "system/Logger.h"
#include "input/MouseCompat.h"
#include "timer/SystemTimerCompat.h"
#include "timer/TickCounter.h"
#include "system/SDLEvent.h"
#include "SDL.h"

#include <functional>

#ifdef _MSC_VER
#pragma warning( disable : 4351 )  
#endif

namespace Equisetum2
{
	class MouseCompat::Impl final
	{
	public:
		Impl()
		{
			m_listener = std::make_shared<SDLEventListener>(this, [this](SDL_Event e, void* key){
				switch (e.type)
				{
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:

					if (e.button.which != SDL_TOUCH_MOUSEID)
					{
						auto button = MouseButton::Max;

						switch (e.button.button)
						{
						case SDL_BUTTON_LEFT:
							button = MouseButton::Left;
							break;
						case SDL_BUTTON_MIDDLE:
							button = MouseButton::Middle;
							break;
						case SDL_BUTTON_RIGHT:
							button = MouseButton::Right;
							break;
						}

						if (button != MouseButton::Max)
						{
							auto& buttonState = m_buttonStateArray[(int)button];

							if (e.type == SDL_MOUSEBUTTONDOWN)
							{
								Logger::OutputInfo("SDL_MOUSEBUTTONDOWN ID %d, button %d\n", e.button.which, (int)button);

								buttonState.down = true;
								buttonState.press = true;
								buttonState.timeStamp = m_timeStamp;
							}
							else if (e.type == SDL_MOUSEBUTTONUP)
							{
								Logger::OutputInfo("SDL_MOUSEBUTTONUP %d\n", (int)button);

								buttonState.up = true;
								buttonState.press = false;
							}

							m_pos.x = e.button.x;
							m_pos.y = e.button.y;
						}
						break;
					}
					break;

				case SDL_MOUSEMOTION:

					if (e.motion.which != SDL_TOUCH_MOUSEID)
					{
						m_pos.x = e.motion.x;
						m_pos.y = e.motion.y;

						m_delta.x += e.motion.xrel;
						m_delta.y += e.motion.yrel;

						Logger::OutputVerbose("SDL_MOUSE MOTION abs(%d, %d) rel(%d, %d)\n", m_pos.x, m_pos.y, m_delta.x, m_delta.y);
					}
					break;

				case SDL_MOUSEWHEEL:

					if (e.wheel.which != SDL_TOUCH_MOUSEID)
					{
						m_wheel = e.wheel.y;	// 垂直方向
						m_wheelH = e.wheel.x;	// 水平方向

						Logger::OutputInfo("SDL_MOUSE WHEEL v %d, h %d\n", m_wheel, m_wheelH);
					}
					break;
				}
			});

			// ボタンの名前を登録
			int i = 0;
			for (auto& state : m_buttonStateArray)
			{
				const char* name[] =
				{
					"MouseButton::Left",
					"MouseButton::Middle",
					"MouseButton::Right",
				};

				state = { name[i] };
				i++;
			}
		}

		void Prepare()
		{
			m_timeStamp = Singleton<SystemTimerCompat>::GetInstance()->Ticks();

			m_delta = {};
			m_wheel = 0;
			m_wheelH = 0;

			// 変化エッジフラグは1フレーム経過したらクリアする
			for (auto& state : m_buttonStateArray)
			{
				state.down = false;
				state.up = false;
			}
		}

		void Update()
		{
			for (auto& state : m_buttonStateArray)
			{
				if (state.press)
				{
					state.pressedDuration = TickCounter::ElapsedTicks(state.timeStamp, m_timeStamp);
				}
			}
		}

		const Point<int32_t>& Pos() const
		{
			return m_pos;
		}

		const Point<int32_t>& Delta() const
		{
			return m_delta;
		}

		int32_t Wheel() const
		{
			return m_wheel;
		}

		int32_t WheelH() const
		{
			return m_wheelH;
		}

		const stKeyState& Button(MouseButton button) const
		{
			return m_buttonStateArray[(int)button];
		}

		bool ShowCursor(bool toggle)
		{
			return !!SDL_ShowCursor(toggle ? SDL_ENABLE : SDL_DISABLE);
		}

	private:

		// SDLイベントリスナー
		std::shared_ptr<SDLEventListener> m_listener;

		// 絶対座標
		Point<int32_t> m_pos = {};
		// 相対座標
		Point<int32_t> m_delta = {};
		// ホイール(垂直)
		int32_t m_wheel = 0;
		// ホイール(水平)
		int32_t m_wheelH = 0;
		// マウスボタン管理配列
		stKeyState m_buttonStateArray[(int)MouseButton::Max] = {};

		// タイムスタンプ
		uint32_t m_timeStamp = 0;
	};
}
