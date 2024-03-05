
#pragma once

#include <ghc/filesystem.hpp>
#include <Geode/loader/Log.hpp>
#include "ScriptInfo.hpp"


namespace LuaManager
{
    inline lua_State* L = nullptr;

void runScript(const ScriptInfo& file);

template <typename... Args>
inline void fatalError(geode::log::impl::FmtStr<Args...> str, Args&&... args) {
    auto e = fmt::format(str, std::forward<Args>(args)...);
    lua_pushstring(L, e.c_str());
    lua_error(L);
}

inline void fatalError(std::string_view str)
{
    fatalError("{}", str);
}

void panic(sol::optional<std::string> maybe_msg);

} //LuaManager