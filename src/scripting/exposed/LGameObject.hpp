#include <sol.hpp>
#include <Geode/binding/GameObject.hpp>

namespace LGameObject
{
    void deleteObjects(const sol::variadic_args& args);
    void addGroup(GameObject* obj, const sol::variadic_args& args);
    void registerGameObject(sol::state&);
    sol::as_table_t<gd::set<short>> getGroups(GameObject* obj);
    void removeAllGroups(GameObject* obj);
    void removeGroups(GameObject*, const sol::variadic_args& args);
    void forAllGroups(GameObject* obj, const sol::function& f);
    void setGroups(GameObject* obj, const sol::variadic_args& args);
    sol::table getProperties(sol::this_state lua, GameObject* obj, const sol::variadic_args& args);
    sol::table getGameObjectTypeTable(sol::state& lua);
}