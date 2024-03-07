#include <sol.hpp>
#include <Geode/binding/GameObject.hpp>

namespace LGameObject
{
    void registerGameObject(sol::state&);
    sol::table getGameObjectTypeTable(sol::state& lua);
}