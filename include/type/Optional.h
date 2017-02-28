#if !defined(_EQOPTIONAL_H_)
#define _EQOPTIONAL_H_

#include "std/optional.hpp"

namespace Equisetum2
{
	// –¼‘O‚ª’·‚Á‚½‚ç‚µ‚¢‚Ì‚ÅAusing‚µ‚Äg‚¤

	template<typename T>
	using Optional = std::experimental::optional<T>;
}

#endif
