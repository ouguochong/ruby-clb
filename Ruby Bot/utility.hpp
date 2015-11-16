#pragma once

#include <sstream>
#include <string>

namespace utility
{
	std::string upper(std::string const& string);
	std::string lower(std::string const& string);
	
	template<class T>
	std::string delimited_numeric(T value)
	{
		std::stringstream ss;
		ss.imbue(std::locale(""));
		ss << std::fixed << value;
		return ss.str();
	}
}