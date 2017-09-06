#if !defined(_EQIKEYSTATE_H_)
#define _EQIKEYSTATE_H_

#include "type/String.h"
#include <cstdint>

namespace Equisetum2
{
	typedef struct
	{
		const char* name;			// キーの名前
		bool down;					// 押下時
		bool up;					// 離した時
		bool press;					// 押されている間
		uint32_t timeStamp;			// 押された時間
		uint32_t pressedDuration;	// 押されている時間
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
