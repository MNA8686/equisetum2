#if !defined(_EQIKEYSTATE_H_)
#define _EQIKEYSTATE_H_

#include "type/String.h"
#include <cstdint>

namespace Equisetum2
{
	typedef struct
	{
		const char* name;			// ƒL[‚Ì–¼‘O
		bool down;					// ‰Ÿ‰º
		bool up;					// —£‚µ‚½
		bool press;					// ‰Ÿ‚³‚ê‚Ä‚¢‚éŠÔ
		uint32_t timeStamp;			// ‰Ÿ‚³‚ê‚½ŠÔ
		uint32_t pressedDuration;	// ‰Ÿ‚³‚ê‚Ä‚¢‚éŠÔ
	}stKeyState;

	class IKeyState
	{
	public:
		virtual const String Name() const = 0;
		virtual bool IsDown() const = 0;
		virtual bool IsUp() const = 0;
		virtual bool IsPress() const = 0;
		virtual uint32_t TimeStamp() const = 0;
		virtual uint32_t PressedDuration() const = 0;
	};
}

#endif
