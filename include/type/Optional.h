#if !defined(_EQOPTIONAL_H_)
#define _EQOPTIONAL_H_

#include "std/optional.hpp"

namespace Equisetum2
{
	// ���O���������炵���̂ŁAusing���Ďg��

	template<typename T>
	using Optional = std::experimental::optional<T>;
}

#endif
