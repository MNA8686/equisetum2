/**
* @file Regex.hpp
* @brief ���K�\���������N���X
* @author MNA
* @date 2017/04/12 �V�K�쐬
*/
#if !defined(_EQREGEX_H_)
#define _EQREGEX_H_

#include "type/String.h"
#include "type/Optional.h"
#include <vector>

namespace Equisetum2
{
	/**
	* ���K�\���������N���X
	*/
	class Regex
	{
	public:
		/**
		* @brief �}�b�`�����������u������(�ŏ��Ƀ}�b�`�������̂̂ݒu��)
		* @param src ���͕�����
		* @param regex �������鐳�K�\��
		* @param replacement �u����̐��K�\��
		* @return �u����̕�����
		*/
		static String ReplaceFirst(const String& src, const String& regex, const String& replacement);

		/**
		* @brief �}�b�`�����������u������
		* @param src ���͕�����
		* @param regex �������鐳�K�\��
		* @param replacement �u����̐��K�\��
		* @return �u����̕�����
		*/
		static String ReplaceAll(const String& src, const String& regex, const String& replacement);

		/**
		* @brief ���������������
		* @param src ���͕�����
		* @param regex �������鐳�K�\��
		* @return ������������
		* @detail �����I�Ɉ�v���镶�����T���ۂɎg�p����B
		*/
		static Optional<std::vector<String>> Search(const String& src, const String& regex);

		/**
		* @brief �����񂪈�v���邩���肷��
		* @param src ���͕�����
		* @param regex �������鐳�K�\��
		* @return ��v����������
		* @detail �S�̂���v���邩�ǂ����𔻒肷��B
		*/
		static Optional<std::vector<String>> Match(const String& src, const String& regex);
	};
}

#endif
