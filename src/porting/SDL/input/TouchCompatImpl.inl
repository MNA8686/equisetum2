#include "system/Logger.h"
#include "input/TouchCompat.h"
#include "timer/SystemTimerCompat.h"
#include "timer/TickCounter.h"
#include "system/SDLEvent.h"
#include "SDL.h"

#ifdef _MSC_VER
#pragma warning( disable : 4351 )  
#endif

namespace Equisetum2
{
	class TouchCompat::Impl final
	{
	public:
		static const int FingerMax = 10;		// このライブラリで扱う最大フィンガー数(iOSが5本までなので念のため、その倍用意しとくｗ)

		Impl()
		{
			m_listener = std::make_shared<SDLEventListener>(this, [this](SDL_Event e, void* key){
				switch (e.type)
				{
				case SDL_FINGERDOWN:
				case SDL_FINGERUP:

					if (e.type == SDL_FINGERDOWN)
					{
						for (auto& state : m_TouchStateArray)
						{
							if (!state.touchState.press &&
								!state.touchState.up)
							{
								state.pos.x = e.tfinger.x;
								state.pos.y = e.tfinger.y;
								state.delta = {};
								state.id = e.tfinger.fingerId;
								state.touchState.down = true;
								state.touchState.press = true;
								state.touchState.timeStamp = m_timeStamp;

//								Logger::OutputInfo("SDL_FINGERDOWN ID %lld (%f, %f)\n", e.tfinger.fingerId, e.tfinger.x, e.tfinger.y);
								break;
							}
						}
					}
					else if (e.type == SDL_FINGERUP)
					{
						for (auto& state : m_TouchStateArray)
						{
							if (state.id == e.tfinger.fingerId &&
								state.touchState.press)
							{
								state.touchState.up = true;
								state.touchState.press = false;
//								Logger::OutputInfo("SDL_FINGERUP %lld\n", e.tfinger.fingerId);
								break;
							}
						}
					}
					break;

				case SDL_FINGERMOTION:

					for (auto& state : m_TouchStateArray)
					{
						if (state.id == e.tfinger.fingerId &&
							state.touchState.press)
						{
							state.pos.x = e.tfinger.x;
							state.pos.y = e.tfinger.y;

							state.delta.x += e.tfinger.dx;
							state.delta.y += e.tfinger.dy;

							Logger::OutputVerbose("SDL_FINGERMOTION MOTION ID %lld, abs(%f, %f) rel(%f, %f)\n", e.tfinger.fingerId, state.pos.x, state.pos.y, state.delta.x, state.delta.y);
							break;
						}
					}
					break;
				}
			});

			// ボタンの名前を登録
			for (auto& state : m_TouchStateArray)
			{
				state = {};
				state.touchState.name = { "Touch Finger" };
			}
		}

		void Prepare()
		{
			m_timeStamp = Singleton<SystemTimerCompat>::GetInstance()->Ticks();

			for (auto& state : m_TouchStateArray)
			{
				state.delta = {};
				state.touchState.down = false;
				state.touchState.up = false;
			}
		}

		void Update()
		{
			for (auto& state : m_TouchStateArray)
			{
				if (state.touchState.press)
				{
					state.touchState.pressedDuration = TickCounter::ElapsedTicks(state.touchState.timeStamp, m_timeStamp);
				}
			}
		}

		const Point<float>& Pos(int fingerIndex) const
		{
			// TODO 無効値判定
			return m_TouchStateArray[fingerIndex].pos;
		}

		const Point<float>& Delta(int fingerIndex) const
		{
			// TODO 無効値判定
			return m_TouchStateArray[fingerIndex].delta;
		}

		const stKeyState& Button(int fingerIndex) const
		{
			// TODO 無効値判定
			return m_TouchStateArray[fingerIndex].touchState;
		}

		int64_t FingerId(int fingerIndex) const
		{
			// TODO 無効値判定
			return m_TouchStateArray[fingerIndex].id;
		}

		int NumSupportedFinger() const
		{
			return FingerMax;
		}

	private:

		// SDLイベントリスナー
		std::shared_ptr<SDLEventListener> m_listener;

		typedef struct
		{
			// フィンガーID
			// フィンガーインデックスとは異なる
			// これは押されるたびにインクリメントされていく(Windowsの場合)
			int64_t id;
			// 絶対座標
			Point<float> pos;
			// 相対座標
			Point<float> delta;
			// タッチ状態管理配列
			stKeyState touchState;
		}stTouchState;

		stTouchState m_TouchStateArray[FingerMax]{};

		// タイムスタンプ
		uint32_t m_timeStamp = 0;
	};
}
