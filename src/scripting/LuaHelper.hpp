#include <sol.hpp>
#include <vector>
#include <string_view>
#include <type_traits>
#include <charconv>
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/GameManager.hpp>


//more like a general helper for my scripting code rather than only lua
namespace LuaHelper
{

inline LevelEditorLayer* editor() {
    return GameManager::sharedState()->m_levelEditorLayer;
}

inline EditorUI* ui() {
    return GameManager::sharedState()->m_levelEditorLayer->m_editorUI;
}

inline GameManager* gm() {
    return GameManager::sharedState();;
}

std::vector<std::string_view> splitByDelimStringView(std::string_view str, char delim);

    template<typename T>
    std::vector<T> getFromVarArgs(const sol::variadic_args& args)
    {
        std::vector<T> ret;
        for(const auto& arg : args)
        {
            if(arg.is<T>())
            {
                ret.push_back(arg.as<T>());
                continue;
            }

            if(arg.is<std::vector<T>>())
            {
                auto v = arg.as<std::vector<T>>();
                ret.insert(ret.end(), v.begin(), v.end());
            }
        }
        return ret;
    }

    template<typename T>
    std::optional<T> fromString(std::string_view str)
    {
    	if constexpr (std::is_same_v<T, bool>)
    	{
    		return str == "1" || str == "true";
    	}
    	else
    	{
    		T value;
    		if (std::from_chars(str.data(), str.data() + str.size(), value).ec == std::errc())
    		{
    			return value;
    		}
    	}
    	return {};
    }

    //https://github.com/ThePhD/sol2/blob/e8e122e9ce46f4f1c0b04003d8b703fe1b89755a/include/sol/utility/to_string.hpp
	inline std::string to_string(const sol::object& object) {
        auto pp = sol::stack::push_pop(object);
        std::size_t len;
		const char* str = luaL_tolstring(object.lua_state(), -1, &len);
		auto result = std::string(str, len);

		// luaL_tolstring pushes the string onto the stack, but since
		// we have copied it into our std::string by now we should
		// remove it from the stack.
		lua_pop(object.lua_state(), 1);
        return result;
	}

    inline bool is_integer(const sol::object& object) {
	    auto pp = sol::stack::push_pop(object);
	    return lua_isinteger(object.lua_state(), -1);
	}



} //namespace LuaHelper