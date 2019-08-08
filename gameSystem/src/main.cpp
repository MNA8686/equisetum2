#include "Equisetum2.h"
#include "Application.hpp"
#include "EqHeap.hpp"

using namespace Equisetum2;

int Main()
{
	Singleton<EqHeap>::GetInstance();

	if (auto app = GetApplication())
	{
		return app->Main();
	}

	return -1;
}
