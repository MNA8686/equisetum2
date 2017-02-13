#if !defined(_EQSYSTEM_H_)
#define _EQSYSTEM_H_

#include <cstdint>

namespace Equisetum2
{
	class System
	{
	public:
		static void Sleep(uint32_t ms);
		static bool Update();
	};
}

#endif
