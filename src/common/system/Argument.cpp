/**
* @file Argument.cpp
* @brief �����擾�N���X
* @author MNA
* @date 2017/10/28 �V�K�쐬
*/

#include "system/Argument.hpp"
#include "system/ArgumentImpl.hpp"

namespace Equisetum2
{
	std::vector<String>::const_iterator Argument::begin()
	{
		return Singleton<ArgumentImpl>::GetInstance()->Get().begin();
	}

	std::vector<String>::const_iterator Argument::end()
	{
		return Singleton<ArgumentImpl>::GetInstance()->Get().end();
	}

	size_t Argument::size()
	{
		return Singleton<ArgumentImpl>::GetInstance()->Get().size();
	}
}
