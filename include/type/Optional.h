#if !defined(_EQOPTIONAL_H_)
#define _EQOPTIONAL_H_

#include "std/optional.hpp"

namespace Equisetum2
{
	// 名前が長ったらしいので、usingして使う

	template<typename T>
	using Optional = std::experimental::optional<T>;
}

#endif
