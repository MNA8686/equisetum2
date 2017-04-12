/**
* @file File.hpp
* @brief �t�@�C������Ɋւ���N���X
* @author MNA
* @date 2017/04/12 �V�K�쐬
*/
#if !defined(_EQFILE_H_)
#define _EQFILE_H_

#include "type/String.h"

namespace Equisetum2
{
	/**
	* �t�@�C������Ɋւ���N���X
	*/
	class File
	{
	public:
		static bool Create(const String& name);
		static bool Delete(const String& name);
		static bool Move(const String& src, const String& dst);
		static bool Exists(const String& name);
		static bool Copy(const String& src, const String& dst, bool overWrite=false);
	};
}

#endif
