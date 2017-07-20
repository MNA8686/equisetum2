#include "system/Singleton.h"

namespace Equisetum2
{
	const int kMaxFinalizersSize = 256;
	static int gNumFinalizersSize = 0;
	static SingletonFinalizer::FinalizerFunc gFinalizers[kMaxFinalizersSize];

	void SingletonFinalizer::AddFinalizer(FinalizerFunc func)
	{
		//		assert(gNumFinalizersSize < kMaxFinalizersSize);
		gFinalizers[gNumFinalizersSize++] = func;
	}

	void SingletonFinalizer::Finalize()
	{
		for (int i = gNumFinalizersSize - 1; i >= 0; --i)
		{
			(*gFinalizers[i])();
		}
		gNumFinalizersSize = 0;
	}
}
