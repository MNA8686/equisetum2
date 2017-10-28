/**
* @file Argument.hpp
* @brief ˆø”æ“¾ƒNƒ‰ƒX
* @author MNA
* @date 2017/10/28 V‹Kì¬
*/
#if !defined(_EQARGUMENT_H_)
#define _EQARGUMENT_H_

#include "type/String.h"

namespace Equisetum2
{
	class Argument final
	{
	public:
		Argument() = default;
		~Argument() = default;

		std::vector<String>::const_iterator begin();

		std::vector<String>::const_iterator end();

		size_t size();
	};
}

#endif
