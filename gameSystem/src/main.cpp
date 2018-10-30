#include "Equisetum2.h"
#include "Application.hpp"

using namespace Equisetum2;


int Main()
{
	if (auto app = GetApplication())
	{
		return app->Main();
	}

	return -1;
}
