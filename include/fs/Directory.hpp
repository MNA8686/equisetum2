/**
* @file Directory.hpp
* @brief �f�B���N�g������Ɋւ���N���X
* @author MNA
* @date 2017/04/12 �V�K�쐬
*/
#if !defined(_EQDIRECTORY_H_)
#define _EQDIRECTORY_H_

#include "type/String.h"
#include "type/Optional.h"

namespace Equisetum2
{
	/**
	* �f�B���N�g������Ɋւ���N���X
	*/
	class Directory
	{
	public:
		static bool Create(const String& name);
		static bool Delete(const String& name, bool recursion=false);
		static bool Move(const String& src, const String& dst);
		static bool Exists(const String& name);

		static Optional<std::vector<String>> GetFiles(const String& name, const String& regex="");
		static Optional<std::vector<String>> GetDirectories(const String& name, const String& regex = "");
		static String GetParent(const String& name);
	};
}

#endif
