#if !defined(_EQIKEYSTATE_H_)
#define _EQIKEYSTATE_H_

#include "type/String.h"
#include <cstdint>

namespace Equisetum2
{
	typedef struct
	{
		const char* name;			// �L�[�̖��O
		bool down;					// ������
		bool up;					// ��������
		bool press;					// ������Ă����
		uint32_t timeStamp;			// �����ꂽ����
		uint32_t pressedDuration;	// ������Ă��鎞��
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
