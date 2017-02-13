#include "system/Logger.h"
#include "input/JoystickInternal.h"
#include "system/SDLEvent.h"
#include "SDL.h"

#include <vector>

#ifdef _MSC_VER
#pragma warning( disable : 4351 )  
#endif

namespace Equisetum2
{
	class SDLJoystick
	{
	public:
		SDLJoystick()
		{
			if (SDL_WasInit(SDL_INIT_JOYSTICK) == 0)
			{
				SDL_InitSubSystem(SDL_INIT_JOYSTICK);
			}
		}

		virtual ~SDLJoystick()
		{
			SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
		}
	};

	class JoystickInternal::Impl final : public SDLJoystick
	{
	public:
		static const int JoyMax = 4;		// このライブラリで扱う最大ジョイスティック数

		Impl()
		{
			m_listener = std::make_shared<SDLEventListener>(this, [this](SDL_Event e, void* key){
				switch (e.type)
				{
				case SDL_JOYDEVICEADDED:
					{
						int32_t joyIndex = e.jdevice.which;		// ジョイスティック番号

						Logger::OutputInfo("SDL_JOYDEVICEADDED %d\n", joyIndex);

						if (joyIndex < JoyMax)
						{
							auto pJoy = SDL_JoystickOpen(joyIndex);
							if (pJoy != nullptr)
							{
								m_JoyState[joyIndex].JoyInstance = std::shared_ptr<SDL_Joystick>(pJoy,
									[](SDL_Joystick* pJoy){
									if (pJoy != nullptr)
									{
										SDL_JoystickClose(pJoy);
									}
								});

								// 軸の数変更＆初期化
								m_JoyState[joyIndex].vAxes.resize(NumAxes(joyIndex));
								for (auto& axis : m_JoyState[joyIndex].vAxes)
								{
									axis = 0;
								}

								// ボタンの数変更＆初期化
								m_JoyState[joyIndex].vKeyState.resize(NumButtons(joyIndex));
								for (auto& keyState : m_JoyState[joyIndex].vKeyState)
								{
									keyState = { "JOY BUTTON" };
								}

								// ハットの数変更＆初期化
								m_JoyState[joyIndex].vHats.resize(NumHats(joyIndex));
								for (auto& hat : m_JoyState[joyIndex].vHats)
								{
									hat = {};
								}

								Logger::OutputInfo("joy %d opened. Axes %d, Buttons %d, Hats %d (%s)\n", joyIndex, NumAxes(joyIndex), NumButtons(joyIndex), NumHats(joyIndex), Name(joyIndex));
							}
						}
					}
					break;

				case SDL_JOYDEVICEREMOVED:
					{
						Logger::OutputInfo("SDL_JOYDEVICEREMOVED\n");
						int i = 0;
						for (auto& inst : m_JoyState)
						{
							if (inst.JoyInstance != nullptr &&
								e.jdevice.which == SDL_JoystickInstanceID(inst.JoyInstance.get()))
							{
								inst.JoyInstance = nullptr;
								Logger::OutputInfo("joy %d closed\n", i);
								break;
							}
							i++;
						}
					}
					break;

				case SDL_JOYBUTTONDOWN:
				case SDL_JOYBUTTONUP:

					for (auto& inst : m_JoyState)
					{
						if (inst.JoyInstance != nullptr &&
							e.jbutton.which == SDL_JoystickInstanceID(inst.JoyInstance.get()))
						{
							auto buttonIndex = e.jbutton.button;	// ボタン番号
							auto& buttonState = inst.vKeyState[buttonIndex];

							if (e.type == SDL_JOYBUTTONDOWN)
							{
								Logger::OutputInfo("SDL_JOYBUTTONDOWN %d\n", buttonIndex);

								buttonState.down = true;
								buttonState.press = true;
								buttonState.pressedDuration = 0;
							}
							else if (e.type == SDL_JOYBUTTONUP)
							{
								Logger::OutputInfo("SDL_JOYBUTTONUP %d\n", buttonIndex);

								buttonState.up = true;
								buttonState.press = false;
							}
							break;
						}
					}
					break;

				case SDL_JOYAXISMOTION:

					for (auto& inst : m_JoyState)
					{
						if (inst.JoyInstance != nullptr &&
							e.jaxis.which == SDL_JoystickInstanceID(inst.JoyInstance.get()))
						{
							auto axisIndex = e.jaxis.axis;	// 軸番号
							inst.vAxes[axisIndex] = e.jaxis.value;

							Logger::OutputInfo("SDL_JOYAXISMOTION %d %d\n", axisIndex, inst.vAxes[axisIndex]);
							break;
						}
					}
					break;

				case SDL_JOYHATMOTION:

					for (auto& inst : m_JoyState)
					{
						if (inst.JoyInstance != nullptr &&
							e.jaxis.which == SDL_JoystickInstanceID(inst.JoyInstance.get()))
						{
							auto hatIndex = e.jhat.hat;	// ハット番号
							auto hatValue = HatState::Centered;

							// SDLの値からEquisetum2の値へ変換
							switch (e.jhat.value)
							{
							case SDL_HAT_RIGHT:
								hatValue = HatState::Right;
								break;
							case SDL_HAT_DOWN:
								hatValue = HatState::Down;
								break;
							case SDL_HAT_LEFT:
								hatValue = HatState::Left;
								break;
							case SDL_HAT_UP:
								hatValue = HatState::Up;
								break;
							case SDL_HAT_RIGHTUP:
								hatValue = HatState::RightUp;
								break;
							case SDL_HAT_RIGHTDOWN:
								hatValue = HatState::RightDown;
								break;
							case SDL_HAT_LEFTUP:
								hatValue = HatState::LeftUp;
								break;
							case SDL_HAT_LEFTDOWN:
								hatValue = HatState::LeftDown;
								break;
							}

							inst.vHats[hatIndex] = hatValue;

							Logger::OutputInfo("SDL_JOYHATMOTION %d %d\n", hatIndex, inst.vHats[hatIndex]);
							break;
						}
					}
					break;
				}
			});
		}

		void Prepare()
		{
			// 変化エッジフラグは1フレーム経過したらクリアする
			for (auto& inst : m_JoyState)
			{
				for (auto& keyState : inst.vKeyState)
				{
					keyState.down = false;
					keyState.up = false;
				}
			}
		}

		void Update()
		{
			for (auto& inst : m_JoyState)
			{
				for (auto& keyState : inst.vKeyState)
				{
					if (keyState.press)
					{
						// カンストしてない？(60FPS換算で1時間までカウントする)
						if (keyState.pressedDuration < 60 * 60 * 60)
						{
							// どれぐらいの時間押されているかをカウント
							keyState.pressedDuration++;
						}
					}
				}
			}
		}

		bool IsConnected(int joyIndex) const
		{
			return (joyIndex < JoyMax &&
				m_JoyState[joyIndex].JoyInstance != nullptr);
		}

		int16_t Axis(int joyIndex, int num) const
		{
			return (IsConnected(joyIndex) && num < (int)m_JoyState[joyIndex].vAxes.size()) ?
				m_JoyState[joyIndex].vAxes[num] : 0;
		}

		HatState Hat(int joyIndex, int num) const
		{
			return (IsConnected(joyIndex) && num < (int)m_JoyState[joyIndex].vHats.size()) ?
				m_JoyState[joyIndex].vHats[num] : HatState::Centered;
		}

		const stKeyState& Button(int joyIndex, int num) const
		{
			static const stKeyState state = { "nul device" };
			const auto* pState = &state;

			if (IsConnected(joyIndex) &&
				num < (int)m_JoyState[joyIndex].vKeyState.size())
			{
				pState = &m_JoyState[joyIndex].vKeyState[num];
			}

			return *pState;
		}

		int NumAxes(int joyIndex) const
		{
			return IsConnected(joyIndex) ? SDL_JoystickNumAxes(m_JoyState[joyIndex].JoyInstance.get()) : 0;
		}

		int NumButtons(int joyIndex) const
		{
			return IsConnected(joyIndex) ? SDL_JoystickNumButtons(m_JoyState[joyIndex].JoyInstance.get()) : 0;
		}

		int NumHats(int joyIndex) const
		{
			return IsConnected(joyIndex) ? SDL_JoystickNumHats(m_JoyState[joyIndex].JoyInstance.get()) : 0;
		}

		const char* Name(int joyIndex) const
		{
			return IsConnected(joyIndex) ? SDL_JoystickName(m_JoyState[joyIndex].JoyInstance.get()) : "null device";
		}

		int NumJoysticks() const
		{
			return SDL_NumJoysticks();
		}

	private:

		// SDLイベントリスナー
		std::shared_ptr<SDLEventListener> m_listener;

		typedef struct
		{
			// ジョイスティックインスタンス
			std::shared_ptr<SDL_Joystick> JoyInstance;
			// キーバッファ
			std::vector<stKeyState> vKeyState;
			// 軸バッファ
			std::vector<int16_t> vAxes;
			// ハットバッファ
			std::vector<HatState> vHats;
		}stJoyState;
		stJoyState m_JoyState[JoyMax] = {};
	};
}
