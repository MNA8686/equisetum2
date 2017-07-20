#include "system/Logger.h"
#include "input/KeyboardCompat.h"
#include "timer/SystemTimerCompat.h"
#include "timer/TickCounter.h"
#include "system/SDLEvent.h"
#include "SDL.h"

#ifdef _MSC_VER
#pragma warning( disable : 4221 4351 )  
#endif

namespace Equisetum2
{
	typedef struct
	{
		SDL_Keycode internalKeycode;
		KeyCode keyCode;
		const char* name;
	}stKeyCode;

	// SDLのキーコードとの変換テーブル
	static const stKeyCode keyCodeTbl[] =
	{
#define KEYCODE_CONV(internalKeycode, keyCode) { internalKeycode, keyCode, #keyCode }
		KEYCODE_CONV(SDLK_UNKNOWN, KeyCode::KEY_UNKNOWN),

		KEYCODE_CONV(SDLK_RETURN, KeyCode::KEY_RETURN),
		KEYCODE_CONV(SDLK_ESCAPE, KeyCode::KEY_ESCAPE),
		KEYCODE_CONV(SDLK_BACKSPACE, KeyCode::KEY_BACKSPACE),
		KEYCODE_CONV(SDLK_TAB, KeyCode::KEY_TAB),
		KEYCODE_CONV(SDLK_SPACE, KeyCode::KEY_SPACE),
		KEYCODE_CONV(SDLK_EXCLAIM, KeyCode::KEY_EXCLAIM),
		KEYCODE_CONV(SDLK_QUOTEDBL, KeyCode::KEY_QUOTEDBL),
		KEYCODE_CONV(SDLK_HASH, KeyCode::KEY_HASH),
		KEYCODE_CONV(SDLK_PERCENT, KeyCode::KEY_PERCENT),
		KEYCODE_CONV(SDLK_DOLLAR, KeyCode::KEY_DOLLAR),
		KEYCODE_CONV(SDLK_AMPERSAND, KeyCode::KEY_AMPERSAND),
		KEYCODE_CONV(SDLK_QUOTE, KeyCode::KEY_QUOTE),
		KEYCODE_CONV(SDLK_LEFTPAREN, KeyCode::KEY_LEFTPAREN),
		KEYCODE_CONV(SDLK_RIGHTPAREN, KeyCode::KEY_RIGHTPAREN),
		KEYCODE_CONV(SDLK_ASTERISK, KeyCode::KEY_ASTERISK),
		KEYCODE_CONV(SDLK_PLUS, KeyCode::KEY_PLUS),
		KEYCODE_CONV(SDLK_COMMA, KeyCode::KEY_COMMA),
		KEYCODE_CONV(SDLK_MINUS, KeyCode::KEY_MINUS),
		KEYCODE_CONV(SDLK_PERIOD, KeyCode::KEY_PERIOD),
		KEYCODE_CONV(SDLK_SLASH, KeyCode::KEY_SLASH),
		KEYCODE_CONV(SDLK_0, KeyCode::KEY_0),
		KEYCODE_CONV(SDLK_1, KeyCode::KEY_1),
		KEYCODE_CONV(SDLK_2, KeyCode::KEY_2),
		KEYCODE_CONV(SDLK_3, KeyCode::KEY_3),
		KEYCODE_CONV(SDLK_4, KeyCode::KEY_4),
		KEYCODE_CONV(SDLK_5, KeyCode::KEY_5),
		KEYCODE_CONV(SDLK_6, KeyCode::KEY_6),
		KEYCODE_CONV(SDLK_7, KeyCode::KEY_7),
		KEYCODE_CONV(SDLK_8, KeyCode::KEY_8),
		KEYCODE_CONV(SDLK_9, KeyCode::KEY_9),
		KEYCODE_CONV(SDLK_COLON, KeyCode::KEY_COLON),
		KEYCODE_CONV(SDLK_SEMICOLON, KeyCode::KEY_SEMICOLON),
		KEYCODE_CONV(SDLK_LESS, KeyCode::KEY_LESS),
		KEYCODE_CONV(SDLK_EQUALS, KeyCode::KEY_EQUALS),
		KEYCODE_CONV(SDLK_GREATER, KeyCode::KEY_GREATER),
		KEYCODE_CONV(SDLK_QUESTION, KeyCode::KEY_QUESTION),
		KEYCODE_CONV(SDLK_AT, KeyCode::KEY_AT),

		KEYCODE_CONV(SDLK_LEFTBRACKET, KeyCode::KEY_LEFTBRACKET),
		KEYCODE_CONV(SDLK_BACKSLASH, KeyCode::KEY_BACKSLASH),
		KEYCODE_CONV(SDLK_RIGHTBRACKET, KeyCode::KEY_RIGHTBRACKET),
		KEYCODE_CONV(SDLK_CARET, KeyCode::KEY_CARET),
		KEYCODE_CONV(SDLK_UNDERSCORE, KeyCode::KEY_UNDERSCORE),
		KEYCODE_CONV(SDLK_BACKQUOTE, KeyCode::KEY_BACKQUOTE),
		KEYCODE_CONV(SDLK_a, KeyCode::KEY_a),
		KEYCODE_CONV(SDLK_b, KeyCode::KEY_b),
		KEYCODE_CONV(SDLK_c, KeyCode::KEY_c),
		KEYCODE_CONV(SDLK_d, KeyCode::KEY_d),
		KEYCODE_CONV(SDLK_e, KeyCode::KEY_e),
		KEYCODE_CONV(SDLK_f, KeyCode::KEY_f),
		KEYCODE_CONV(SDLK_g, KeyCode::KEY_g),
		KEYCODE_CONV(SDLK_h, KeyCode::KEY_h),
		KEYCODE_CONV(SDLK_i, KeyCode::KEY_i),
		KEYCODE_CONV(SDLK_j, KeyCode::KEY_j),
		KEYCODE_CONV(SDLK_k, KeyCode::KEY_k),
		KEYCODE_CONV(SDLK_l, KeyCode::KEY_l),
		KEYCODE_CONV(SDLK_m, KeyCode::KEY_m),
		KEYCODE_CONV(SDLK_n, KeyCode::KEY_n),
		KEYCODE_CONV(SDLK_o, KeyCode::KEY_o),
		KEYCODE_CONV(SDLK_p, KeyCode::KEY_p),
		KEYCODE_CONV(SDLK_q, KeyCode::KEY_q),
		KEYCODE_CONV(SDLK_r, KeyCode::KEY_r),
		KEYCODE_CONV(SDLK_s, KeyCode::KEY_s),
		KEYCODE_CONV(SDLK_t, KeyCode::KEY_t),
		KEYCODE_CONV(SDLK_u, KeyCode::KEY_u),
		KEYCODE_CONV(SDLK_v, KeyCode::KEY_v),
		KEYCODE_CONV(SDLK_w, KeyCode::KEY_w),
		KEYCODE_CONV(SDLK_x, KeyCode::KEY_x),
		KEYCODE_CONV(SDLK_y, KeyCode::KEY_y),
		KEYCODE_CONV(SDLK_z, KeyCode::KEY_z),

		KEYCODE_CONV(SDLK_CAPSLOCK, KeyCode::KEY_CAPSLOCK),

		KEYCODE_CONV(SDLK_F1, KeyCode::KEY_F1),
		KEYCODE_CONV(SDLK_F2, KeyCode::KEY_F2),
		KEYCODE_CONV(SDLK_F3, KeyCode::KEY_F3),
		KEYCODE_CONV(SDLK_F4, KeyCode::KEY_F4),
		KEYCODE_CONV(SDLK_F5, KeyCode::KEY_F5),
		KEYCODE_CONV(SDLK_F6, KeyCode::KEY_F6),
		KEYCODE_CONV(SDLK_F7, KeyCode::KEY_F7),
		KEYCODE_CONV(SDLK_F8, KeyCode::KEY_F8),
		KEYCODE_CONV(SDLK_F9, KeyCode::KEY_F9),
		KEYCODE_CONV(SDLK_F10, KeyCode::KEY_F10),
		KEYCODE_CONV(SDLK_F11, KeyCode::KEY_F11),
		KEYCODE_CONV(SDLK_F12, KeyCode::KEY_F12),

		KEYCODE_CONV(SDLK_PRINTSCREEN, KeyCode::KEY_PRINTSCREEN),
		KEYCODE_CONV(SDLK_SCROLLLOCK, KeyCode::KEY_SCROLLLOCK),
		KEYCODE_CONV(SDLK_PAUSE, KeyCode::KEY_PAUSE),
		KEYCODE_CONV(SDLK_INSERT, KeyCode::KEY_INSERT),
		KEYCODE_CONV(SDLK_HOME, KeyCode::KEY_HOME),
		KEYCODE_CONV(SDLK_PAGEUP, KeyCode::KEY_PAGEUP),
		KEYCODE_CONV(SDLK_DELETE, KeyCode::KEY_DELETE),
		KEYCODE_CONV(SDLK_END, KeyCode::KEY_END),
		KEYCODE_CONV(SDLK_PAGEDOWN, KeyCode::KEY_PAGEDOWN),
		KEYCODE_CONV(SDLK_RIGHT, KeyCode::KEY_RIGHT),
		KEYCODE_CONV(SDLK_LEFT, KeyCode::KEY_LEFT),
		KEYCODE_CONV(SDLK_DOWN, KeyCode::KEY_DOWN),
		KEYCODE_CONV(SDLK_UP, KeyCode::KEY_UP),

		KEYCODE_CONV(SDLK_NUMLOCKCLEAR, KeyCode::KEY_NUMLOCKCLEAR),
		KEYCODE_CONV(SDLK_KP_DIVIDE, KeyCode::KEY_KP_DIVIDE),
		KEYCODE_CONV(SDLK_KP_MULTIPLY, KeyCode::KEY_KP_MULTIPLY),
		KEYCODE_CONV(SDLK_KP_MINUS, KeyCode::KEY_KP_MINUS),
		KEYCODE_CONV(SDLK_KP_PLUS, KeyCode::KEY_KP_PLUS),
		KEYCODE_CONV(SDLK_KP_ENTER, KeyCode::KEY_KP_ENTER),
		KEYCODE_CONV(SDLK_KP_1, KeyCode::KEY_KP_1),
		KEYCODE_CONV(SDLK_KP_2, KeyCode::KEY_KP_2),
		KEYCODE_CONV(SDLK_KP_3, KeyCode::KEY_KP_3),
		KEYCODE_CONV(SDLK_KP_4, KeyCode::KEY_KP_4),
		KEYCODE_CONV(SDLK_KP_5, KeyCode::KEY_KP_5),
		KEYCODE_CONV(SDLK_KP_6, KeyCode::KEY_KP_6),
		KEYCODE_CONV(SDLK_KP_7, KeyCode::KEY_KP_7),
		KEYCODE_CONV(SDLK_KP_8, KeyCode::KEY_KP_8),
		KEYCODE_CONV(SDLK_KP_9, KeyCode::KEY_KP_9),
		KEYCODE_CONV(SDLK_KP_0, KeyCode::KEY_KP_0),
		KEYCODE_CONV(SDLK_KP_PERIOD, KeyCode::KEY_KP_PERIOD),

		KEYCODE_CONV(SDLK_APPLICATION, KeyCode::KEY_APPLICATION),
		KEYCODE_CONV(SDLK_POWER, KeyCode::KEY_POWER),
		KEYCODE_CONV(SDLK_KP_EQUALS, KeyCode::KEY_KP_EQUALS),


		KEYCODE_CONV(SDLK_LCTRL, KeyCode::KEY_LCTRL),
		KEYCODE_CONV(SDLK_LSHIFT, KeyCode::KEY_LSHIFT),
		KEYCODE_CONV(SDLK_LALT, KeyCode::KEY_LALT),
		KEYCODE_CONV(SDLK_LGUI, KeyCode::KEY_LGUI),
		KEYCODE_CONV(SDLK_RCTRL, KeyCode::KEY_RCTRL),
		KEYCODE_CONV(SDLK_RSHIFT, KeyCode::KEY_RSHIFT),
		KEYCODE_CONV(SDLK_RALT, KeyCode::KEY_RALT),
		KEYCODE_CONV(SDLK_RGUI, KeyCode::KEY_RGUI),

		KEYCODE_CONV(SDLK_MODE, KeyCode::KEY_MODE),
#undef KEYCODE_CONV
	};

	class KeyboardCompat::Impl final
	{
	public:
		Impl()
		{
			m_listener = std::make_shared<SDLEventListener>(this, [this](SDL_Event e, void* key){
				switch (e.type)
				{
				case SDL_KEYDOWN:
				case SDL_KEYUP:
					for (int i = 0; i < (int)KeyCode::KEY_MAX; i++)
					{
						if (e.key.keysym.sym == keyCodeTbl[i].internalKeycode)
						{
							if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
							{
								m_keyStateArray[i].down = true;
								m_keyStateArray[i].press = true;
								m_keyStateArray[i].timeStamp = m_timeStamp;
							}
							else if (e.type == SDL_KEYUP)
							{
								m_keyStateArray[i].up = true;
								m_keyStateArray[i].press = false;
							}
							break;
						}
					}
					break;
				}
			});

			for (int i = 0; i < (int)KeyCode::KEY_MAX; i++)
			{
				m_keyStateArray[i].name = keyCodeTbl[i].name;
			}
		}

		void Prepare()
		{
			m_timeStamp = Singleton<SystemTimerCompat>::GetInstance()->Ticks();

			// 変化エッジフラグは1フレーム経過したらクリアする
			for (auto& keyState : m_keyStateArray)
			{
				keyState.down = false;
				keyState.up = false;
			}
		}

		void Update()
		{
			for (auto& keyState : m_keyStateArray)
			{
				if (keyState.press)
				{
					keyState.pressedDuration = TickCounter::ElapsedTicks(keyState.timeStamp, m_timeStamp);
				}
			}
		}

		const stKeyState& GetKeyState(KeyCode keyCode) const
		{
			return m_keyStateArray[(int)keyCode];
		}

		const char* GetNameFromKeyCode(KeyCode keyCode) const
		{
			return keyCodeTbl[(int)keyCode].name;
		}

	private:

		// SDLイベントリスナー
		std::shared_ptr<SDLEventListener> m_listener;
		// キーバッファ
		stKeyState m_keyStateArray[(int)KeyCode::KEY_MAX] = {};
		// タイムスタンプ
		uint32_t m_timeStamp = 0;
	};
}

