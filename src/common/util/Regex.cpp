/**
* @file Regex.cpp
* @brief 正規表現を扱うクラス
* @author MNA
* @date 2017/04/12 新規作成
*/

#include "util/Regex.hpp"
#include <regex>

namespace Equisetum2
{
	String Regex::ReplaceFirst(const String& input, const String& regex, const String& replacement)
	{
		std::regex re(regex);
		return std::regex_replace(input.c_str(), re, replacement.c_str(), std::regex_constants::format_first_only);
	}

	String Regex::ReplaceAll(const String& input, const String& regex, const String& replacement)
	{
		std::regex re(regex);
		return std::regex_replace(input.c_str(), re, replacement.c_str());
	}

	Optional<std::vector<String>> Regex::Search(const String& input, const String& regex)
	{
		Optional<std::vector<String>> ret;
		std::regex re(regex);
		std::cmatch match;

		if (std::regex_search(input.c_str(), match, re))
		{
			std::vector<String> v;

			// キャプチャ結果などをvectorに格納する
			for (const auto& submatch : match)
			{
				v.push_back(String(submatch));
			}

			ret = v;
		}
		return ret;
	}

	Optional<std::vector<String>> Regex::Match(const String& input, const String& regex)
	{
		Optional<std::vector<String>> ret;
		std::regex re(regex);
		std::cmatch match;

		if (std::regex_match(input.c_str(), match, re))
		{
			std::vector<String> v;

			// キャプチャ結果などをvectorに格納する
			for (const auto& submatch : match)
			{
				v.push_back(String(submatch));
			}

			ret = v;
		}
		return ret;
	}
}

