﻿#if !defined(_EQKEYCODE_H_)
#define _EQKEYCODE_H_

namespace Equisetum2
{
	enum class KeyCode : int
	{
		KEY_UNKNOWN,

		KEY_RETURN,		// '\r'
		KEY_ESCAPE,		// '\033'
		KEY_BACKSPACE,	// '\b'
		KEY_TAB,		// '\t'
		KEY_SPACE,		// ' '
		KEY_EXCLAIM,	// '!'
		KEY_QUOTEDBL,	// '"'
		KEY_HASH,		// '#'
		KEY_PERCENT,	// '%'
		KEY_DOLLAR,		// '$'
		KEY_AMPERSAND,	// '&'
		KEY_QUOTE,		// '''
		KEY_LEFTPAREN,	// '('
		KEY_RIGHTPAREN,	// ')'
		KEY_ASTERISK,	// '*'
		KEY_PLUS,		// '+'
		KEY_COMMA,		// ','
		KEY_MINUS,		// '-'
		KEY_PERIOD,		// '.'
		KEY_SLASH,		// '/'
		KEY_0,		// '0'
		KEY_1,		// '1'
		KEY_2,		// '2'
		KEY_3,		// '3'
		KEY_4,		// '4'
		KEY_5,		// '5'
		KEY_6,		// '6'
		KEY_7,		// '7'
		KEY_8,		// '8'
		KEY_9,		// '9'
		KEY_COLON,		// ':'
		KEY_SEMICOLON,	// ';'
		KEY_LESS,		// '<'
		KEY_EQUALS,		// '='
		KEY_GREATER,	// '>'
		KEY_QUESTION,	// '?'
		KEY_AT,		// '@'

		KEY_LEFTBRACKET,	// '['
		KEY_BACKSLASH,	// '\\'
		KEY_RIGHTBRACKET,	// ']'
		KEY_CARET,		// '^'
		KEY_UNDERSCORE,	// '_'
		KEY_BACKQUOTE,	// '`'
		KEY_a,		// 'a'
		KEY_b,		// 'b'
		KEY_c,		// 'c'
		KEY_d,		// 'd'
		KEY_e,		// 'e'
		KEY_f,		// 'f'
		KEY_g,		// 'g'
		KEY_h,		// 'h'
		KEY_i,		// 'i'
		KEY_j,		// 'j'
		KEY_k,		// 'k'
		KEY_l,		// 'l'
		KEY_m,		// 'm'
		KEY_n,		// 'n'
		KEY_o,		// 'o'
		KEY_p,		// 'p'
		KEY_q,		// 'q'
		KEY_r,		// 'r'
		KEY_s,		// 's'
		KEY_t,		// 't'
		KEY_u,		// 'u'
		KEY_v,		// 'v'
		KEY_w,		// 'w'
		KEY_x,		// 'x'
		KEY_y,		// 'y'
		KEY_z,		// 'z'

		KEY_CAPSLOCK,

		KEY_F1,
		KEY_F2,
		KEY_F3,
		KEY_F4,
		KEY_F5,
		KEY_F6,
		KEY_F7,
		KEY_F8,
		KEY_F9,
		KEY_F10,
		KEY_F11,
		KEY_F12,

		KEY_PRINTSCREEN,
		KEY_SCROLLLOCK,
		KEY_PAUSE,
		KEY_INSERT,
		KEY_HOME,
		KEY_PAGEUP,
		KEY_DELETE,		// '\177'
		KEY_END,
		KEY_PAGEDOWN,
		KEY_RIGHT,
		KEY_LEFT,
		KEY_DOWN,
		KEY_UP,

		KEY_NUMLOCKCLEAR,
		KEY_KP_DIVIDE,
		KEY_KP_MULTIPLY,
		KEY_KP_MINUS,
		KEY_KP_PLUS,
		KEY_KP_ENTER,
		KEY_KP_1,
		KEY_KP_2,
		KEY_KP_3,
		KEY_KP_4,
		KEY_KP_5,
		KEY_KP_6,
		KEY_KP_7,
		KEY_KP_8,
		KEY_KP_9,
		KEY_KP_0,
		KEY_KP_PERIOD,

		KEY_APPLICATION,
		KEY_POWER,
		KEY_KP_EQUALS,
		//    KEY_F13,
		//    KEY_F14,
		//    KEY_F15,
		//    KEY_F16,
		//    KEY_F17,
		//    KEY_F18,
		//    KEY_F19,
		//    KEY_F20,
		//    KEY_F21,
		//    KEY_F22,
		//    KEY_F23,
		//    KEY_F24,
		//    KEY_EXECUTE,
		//    KEY_HELP,
		//    KEY_MENU,
		//    KEY_SELECT,
		//    KEY_STOP,
		//    KEY_AGAIN,
		//    KEY_UNDO,
		//    KEY_CUT,
		//    KEY_COPY,
		//    KEY_PASTE,
		//    KEY_FIND,
		//    KEY_MUTE,
		//    KEY_VOLUMEUP,
		//    KEY_VOLUMEDOWN,
		KEY_KP_COMMA,
		//    KEY_KP_EQUALSAS400,

		//    KEY_ALTERASE,
		//    KEY_SYSREQ,
		//    KEY_CANCEL,
		//    KEY_CLEAR,
		//    KEY_PRIOR,
		//    KEY_RETURN2,
		//    KEY_SEPARATOR,
		//    KEY_OUT,
		//    KEY_OPER,
		//    KEY_CLEARAGAIN,
		//    KEY_CRSEL,
		//    KEY_EXSEL,

		//    KEY_KP_00,
		//    KEY_KP_000,
		//    KEY_THOUSANDSSEPARATOR,
		//    KEY_DECIMALSEPARATOR,
		//    KEY_CURRENCYUNIT,
		//    KEY_CURRENCYSUBUNIT,
		//    KEY_KP_LEFTPAREN = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_LEFTPAREN),
		//    KEY_KP_RIGHTPAREN = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_RIGHTPAREN),
		//    KEY_KP_LEFTBRACE = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_LEFTBRACE),
		//    KEY_KP_RIGHTBRACE = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_RIGHTBRACE),
		//    KEY_KP_TAB = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_TAB),
		//    KEY_KP_BACKSPACE = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_BACKSPACE),
		//    KEY_KP_A = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_A),
		//    KEY_KP_B = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_B),
		//    KEY_KP_C = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_C),
		//    KEY_KP_D = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_D),
		//    KEY_KP_E = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_E),
		//    KEY_KP_F = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_F),
		//    KEY_KP_XOR = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_XOR),
		//    KEY_KP_POWER = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_POWER),
		//    KEY_KP_PERCENT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PERCENT),
		//    KEY_KP_LESS = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_LESS),
		//    KEY_KP_GREATER = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_GREATER),
		//    KEY_KP_AMPERSAND = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_AMPERSAND),
		//    KEY_KP_DBLAMPERSAND =
		//        SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DBLAMPERSAND),
		//    KEY_KP_VERTICALBAR =
		//        SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_VERTICALBAR),
		//    KEY_KP_DBLVERTICALBAR =
		//        SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DBLVERTICALBAR),
		//    KEY_KP_COLON = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_COLON),
		//    KEY_KP_HASH = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_HASH),
		//    KEY_KP_SPACE = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_SPACE),
		//    KEY_KP_AT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_AT),
		//    KEY_KP_EXCLAM = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_EXCLAM),
		//    KEY_KP_MEMSTORE = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMSTORE),
		//    KEY_KP_MEMRECALL = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMRECALL),
		//    KEY_KP_MEMCLEAR = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMCLEAR),
		//    KEY_KP_MEMADD = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMADD),
		//    KEY_KP_MEMSUBTRACT =
		//        SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMSUBTRACT),
		//    KEY_KP_MEMMULTIPLY =
		//        SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMMULTIPLY),
		//    KEY_KP_MEMDIVIDE = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMDIVIDE),
		//    KEY_KP_PLUSMINUS = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PLUSMINUS),
		//    KEY_KP_CLEAR = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_CLEAR),
		//    KEY_KP_CLEARENTRY = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_CLEARENTRY),
		//    KEY_KP_BINARY = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_BINARY),
		//    KEY_KP_OCTAL = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_OCTAL),
		//    KEY_KP_DECIMAL = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DECIMAL),
		//    KEY_KP_HEXADECIMAL =
		//        SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_HEXADECIMAL),

		KEY_LCTRL,
		KEY_LSHIFT,
		KEY_LALT,
		KEY_LGUI,		// Windowsキー
		KEY_RCTRL,
		KEY_RSHIFT,
		KEY_RALT,
		KEY_RGUI,		// Windowsキー

		KEY_MODE,

		//    KEY_AUDIONEXT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIONEXT),
		//    KEY_AUDIOPREV = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOPREV),
		//    KEY_AUDIOSTOP = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOSTOP),
		//    KEY_AUDIOPLAY = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOPLAY),
		//    KEY_AUDIOMUTE = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AUDIOMUTE),
		//    KEY_MEDIASELECT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIASELECT),
		//    KEY_WWW = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_WWW),
		//    KEY_MAIL = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MAIL),
		//    KEY_CALCULATOR = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CALCULATOR),
		//    KEY_COMPUTER = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_COMPUTER),
		//    KEY_AC_SEARCH = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_SEARCH),
		//    KEY_AC_HOME = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_HOME),
		//    KEY_AC_BACK = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_BACK),
		//    KEY_AC_FORWARD = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_FORWARD),
		//    KEY_AC_STOP = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_STOP),
		//    KEY_AC_REFRESH = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_REFRESH),
		//    KEY_AC_BOOKMARKS = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_BOOKMARKS),

		//    KEY_BRIGHTNESSDOWN,
		//    KEY_BRIGHTNESSUP,
		//    KEY_DISPLAYSWITCH,
		//    KEY_KBDILLUMTOGGLE,
		//    KEY_KBDILLUMDOWN,
		//    KEY_KBDILLUMUP,
		//    KEY_EJECT,
		//    KEY_SLEEP

		KEY_MAX
	};
}

#endif
