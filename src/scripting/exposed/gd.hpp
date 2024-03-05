#include <sol.hpp>
#include <Geode/binding/GameObject.hpp>
#include <vector>


namespace gd
{
std::vector<GameObject*> getAllObjects();
std::vector<GameObject*> getSelectedObjects();
void forAllObjects(const sol::function& f);
void forSelectedObjects(const sol::function& f);
GameObject* createObject(const sol::lua_table& t);
void deleteObjects(const sol::variadic_args& args);
void setObjectPropertyEnums(sol::state& lua);

void registerTable(sol::state&);
} //gd