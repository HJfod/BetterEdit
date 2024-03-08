#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/utils/cocos.hpp>

#include <sol.hpp>

#include "LuaManager.hpp"
#include "LuaHelper.hpp"
#include <matjson.hpp>
#include <ghc/filesystem.hpp>
#include "exposed/gd.hpp"
#include "exposed/LGameObject.hpp"

using namespace cocos2d;

void appendToString(std::string_view separator, std::string& out, const sol::variadic_args& args)
{
    for(const auto& arg : args)
    {
        out += separator;
        out += LuaHelper::to_string(arg);
    }
}

void LuaManager::panic(sol::optional<std::string> maybe_msg)
{
    if(maybe_msg)
    {
        geode::log::error("A lua panic occurred {}", *maybe_msg);
    }
    else
    {
        geode::log::error("A lua panic ocurred but no error was found");
    }
}


sol::protected_function_result internal_error(lua_State*, sol::protected_function_result pfr)
{
    sol::error err = pfr;
    geode::log::error("An error occurred {}", err.what());
    return pfr;
}

void LuaManager::runScript(const ScriptInfo& scriptinfo)
{
    sol::state lua;
    LuaManager::L = lua.lua_state();

    lua.open_libraries(
        sol::lib::base,
        sol::lib::math,
        sol::lib::table,
        sol::lib::string,
        sol::lib::package
    );
    LGameObject::registerGameObject(lua);
    gd::registerTable(lua);
    lua.set("GameObjectType", LGameObject::getGameObjectTypeTable(lua));

    gd::setObjectPropertyEnums(lua);
    //for(const auto& nameTablePair : gd::getPropertyTable(lua))
    //{
    //    lua.set(nameTablePair);
    //}

    std::string tostdout;
    std::string toappend = fmt::format("\n[{}]: ", scriptinfo.name);
    sol::table gd = lua["gd"];
    gd.set("print", [&](const sol::variadic_args& args) {
        tostdout += toappend;
        appendToString(" ", tostdout, args);
    });

    lua.safe_script_file(scriptinfo.path.string(), internal_error);

    if(!tostdout.empty())
    {
        if(tostdout.back() == '\n')
           tostdout.pop_back();
        geode::log::info("{}", tostdout);
    }


}