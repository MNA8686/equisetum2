#include "system/Logger.h"
#include "input/JoystickInternal.h"
#include "timer/SystemTimerInternal.h"
#include "timer/TickCounter.h"
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
		static const int JoyMax = 4;		// ���̃��C�u�����ň����ő�W���C�X�e�B�b�N��

		Impl()
		{
			m_listener = std::make_shared<SDLEventListener>(this, [this](SDL_Event e, void* key){
				switch (e.type)
				{
				case SDL_JOYDEVICEADDED:
					{
						int32_t joyIndex = e.jdevice.which;		// �W���C�X�e�B�b�N�ԍ�

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

								// ���̐��ύX��������
								m_JoyState[joyIndex].vAxes.resize(NumAxes(joyIndex));
								for (auto& axis : m_JoyState[joyIndex].vAxes)
								{
									axis = 0;
								}

								// �{�^���̐��ύX��������
								m_JoyState[joyIndex].vKeyState.resize(NumButtons(joyIndex));
								for (auto& keyState : m_JoyState[joyIndex].vKeyState)
								{
									keyState = { "JOY BUTTON" };
								}

								// �n�b�g�̐��ύX��������
								m_JoyState[joyIndex].vHats.resize(NumHats(joyIndex));
								for (auto& hat : m_JoyState[joyIndex].vHats)
								{
									for (auto& keyState : hat.state)
									{
										keyState = {};
									}
								}

								Logger::OutputInfo("joy %d opened. Axes %d, Buttons %d, Hats %d\n", joyIndex, NumAxes(joyIndex), NumButtons(joyIndex), NumHats(joyIndex));
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
							auto buttonIndex = e.jbutton.button;	// �{�^���ԍ�
							auto& buttonState = inst.vKeyState[buttonIndex];

							if (e.type == SDL_JOYBUTTONDOWN)
							{
								Logger::OutputInfo("SDL_JOYBUTTONDOWN %d\n", buttonIndex);

								buttonState.down = true;
								buttonState.press = true;
								buttonState.timeStamp = m_timeStamp;
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
							auto axisIndex = e.jaxis.axis;	// ���ԍ�
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
							auto hatIndex = e.jhat.hat;	// �n�b�g�ԍ�
							auto update = [hatIndex, &inst, e](eHatState hatState, int mask)
							{
								auto& key = inst.vHats[hatIndex].state[(int)hatState];
								if (e.jhat.value & mask)
								{
									if (!key.press)
									{
										key.down = true;
									}
									key.press = true;
								}
								else
								{
									if (key.press)
									{
										key.up = true;
									}
									key.press = false;
								}
							};

							update(eHatState::Left, SDL_HAT_LEFT);
							update(eHatState::Right, SDL_HAT_RIGHT);
							update(eHatState::Up, SDL_HAT_UP);
							update(eHatState::Down, SDL_HAT_DOWN);

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
			m_timeStamp = Singleton<SystemTimerInternal>::GetInstance()->Ticks();

			// �ω��G�b�W�t���O��1�t���[���o�߂�����N���A����
			for (auto& inst : m_JoyState)
			{
				// �{�^��
				for (auto& keyState : inst.vKeyState)
				{
					keyState.down = false;
					keyState.up = false;
				}

				// �n�b�g
				for (auto& keyState : inst.vHats)
				{
					for (auto& key : keyState.state)
					{
						key.down = false;
						key.up = false;
					}
				}
			}
		}

		void Update()
		{
			for (auto& inst : m_JoyState)
			{
				// �{�^��
				for (auto& keyState : inst.vKeyState)
				{
					if (keyState.press)
					{
						keyState.pressedDuration = TickCounter::ElapsedTicks(keyState.timeStamp, m_timeStamp);
					}
				}

				// �n�b�g
				for (auto& keyState : inst.vHats)
				{
					for (auto& keyState_ : keyState.state)
					{
						if (keyState_.press)
						{
							keyState_.pressedDuration = TickCounter::ElapsedTicks(keyState_.timeStamp, m_timeStamp);
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

		const stKeyState& HatUp(int joyIndex, int num) const
		{
			const auto* pState = &m_nullState;

			if (IsConnected(joyIndex))
			{
				pState = &m_JoyState[joyIndex].vHats[num].state[(int)eHatState::Up];
			}

			return *pState;
		}

		const stKeyState& HatDown(int joyIndex, int num) const
		{
			const auto* pState = &m_nullState;

			if (IsConnected(joyIndex))
			{
				pState = &m_JoyState[joyIndex].vHats[num].state[(int)eHatState::Down];
			}

			return *pState;
		}

		const stKeyState& HatLeft(int joyIndex, int num) const
		{
			const auto* pState = &m_nullState;

			if (IsConnected(joyIndex))
			{
				pState = &m_JoyState[joyIndex].vHats[num].state[(int)eHatState::Left];
			}

			return *pState;
		}

		const stKeyState& HatRight(int joyIndex, int num) const
		{
			const auto* pState = &m_nullState;

			if (IsConnected(joyIndex))
			{
				pState = &m_JoyState[joyIndex].vHats[num].state[(int)eHatState::Right];
			}

			return *pState;
		}

		const stKeyState& Button(int joyIndex, int num) const
		{
			const auto* pState = &m_nullState;

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

		// SDL�C�x���g���X�i�[
		std::shared_ptr<SDLEventListener> m_listener;

		// �n�b�g�X�C�b�`�̒�`
		enum class eHatState : int
		{
			Right,
			Up,
			Left,
			Down,
			Max,
		};

		typedef struct
		{
			stKeyState state[(int)eHatState::Max];
		}stHatState;

		typedef struct
		{
			// �W���C�X�e�B�b�N�C���X�^���X
			std::shared_ptr<SDL_Joystick> JoyInstance;
			// �L�[�o�b�t�@
			std::vector<stKeyState> vKeyState;
			// ���o�b�t�@
			std::vector<int16_t> vAxes;
			// �n�b�g�o�b�t�@
			std::vector<stHatState> vHats;
		}stJoyState;
		stJoyState m_JoyState[JoyMax] = {};

		// �^�C���X�^���v
		uint32_t m_timeStamp = 0;

		static const stKeyState m_nullState;
	};

	const stKeyState JoystickInternal::Impl::m_nullState = { "nul device" };
}
