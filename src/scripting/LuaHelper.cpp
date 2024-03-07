
#include "LuaHelper.hpp"
#include <vector>
#include <string_view>

std::vector<std::string_view> LuaHelper::splitByDelimStringView(std::string_view str, char delim)
{
	std::vector<std::string_view> tokens;
	size_t pos = 0;
	size_t len = str.length();

	while (pos < len)
	{
		size_t end = str.find(delim, pos);
		if (end == std::string_view::npos)
		{
			tokens.emplace_back(str.substr(pos));
			break;
		}
		tokens.emplace_back(str.substr(pos, end - pos));
		pos = end + 1;
	}
	return tokens;
}