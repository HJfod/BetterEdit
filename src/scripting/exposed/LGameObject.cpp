#include "LGameObject.hpp"
#include <sol.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <scripting/LuaHelper.hpp>
#include <Geode/utils/cocos.hpp>

using namespace cocos2d;
using namespace geode;

static gd::set<short> getGroupIDs(GameObject* obj) {
    gd::set<short> res;

    if (obj->m_groups && obj->m_groups->at(0))
        for (auto i = 0; i < obj->m_groupCount; i++)
            res.insert(obj->m_groups->at(i));

    return res;
}

namespace
{


void addGroup(GameObject* obj, const sol::variadic_args& args)
{
    for(const auto& g : LuaHelper::getFromVarArgs<int>(args))
    {
        obj->addToGroup(g);
    }
}

sol::as_table_t<gd::set<short>> getGroups(GameObject* obj)
{
    return sol::as_table(getGroupIDs(obj));
}

void removeAllGroups(GameObject* obj)
{
    for(const auto& g : getGroupIDs(obj))
    {
        obj->removeFromGroup(static_cast<int>(g));
    }
}

void removeGroups(GameObject* obj, const sol::variadic_args& args)
{
    for(const auto& g : LuaHelper::getFromVarArgs<int>(args))
    {
        obj->removeFromGroup(g);
    }
}

void forAllGroups(GameObject* obj, const sol::function& f)
{
    for(const auto& g : getGroupIDs(obj))
    {
        f(static_cast<int>(g));
    }
}

void setGroups(GameObject* obj, const sol::variadic_args& args)
{
    removeAllGroups(obj);
    addGroup(obj, args);
}

sol::table getProperties(sol::this_state lua, GameObject* obj, const sol::variadic_args& args)
{
    std::vector<int> v = LuaHelper::getFromVarArgs<int>(args);
    std::string str = obj->getSaveString(LuaHelper::editor());
    std::vector<std::string_view> objectProps = LuaHelper::splitByDelimStringView(str, ',');

    sol::table t = sol::state_view(lua).create_table();

    for(auto it = objectProps.begin(); it < objectProps.end(); it += 2)
    {
        int key = 0;
        if(auto val = LuaHelper::fromString<int>(*it)) key = *val;
        else continue;

        auto valueIt = it + 1;
        if(auto val = LuaHelper::fromString<int>(*valueIt))
            t.set(key, *val);
        else if(auto val = LuaHelper::fromString<float>(*valueIt))
            t.set(key, *val);
        else
            t.set(key, std::string(*valueIt));
    }
    return t;
}

} //empty namespace


//this reorders sections
inline void moveObjectX(GameObject* obj, float x)
{
    LuaHelper::ui()->moveObject(obj, CCPoint(x - obj->getPositionX(), 0.0f));
}
inline void moveObjectY(GameObject* obj, float y)
{
    LuaHelper::ui()->moveObject(obj, CCPoint(0.0f, y - obj->getPositionY()));
}

sol::table LGameObject::getGameObjectTypeTable(sol::state& lua)
{
    return lua.create_table_with
    (
        "Solid", 0,
        "Hazard", 2,
        "InverseGravityPortal", 3,
        "NormalGravityPortal", 4,
        "ShipPortal", 5,
        "CubePortal", 6,
        "Decoration", 7,
        "YellowJumpPad", 8,
        "PinkJumpPad", 9,
        "GravityPad", 10,
        "YellowJumpRing", 11,
        "PinkJumpRing", 12,
        "GravityRing", 13,
        "InverseMirrorPortal", 14,
        "NormalMirrorPortal", 15,
        "BallPortal", 16,
        "RegularSizePortal", 17,
        "MiniSizePortal", 18,
        "UfoPortal", 19,
        "Modifier", 20,
        "Breakable", 21,
        "SecretCoin", 22,
        "DualPortal", 23,
        "SoloPortal", 24,
        "Slope", 25,
        "WavePortal", 26,
        "RobotPortal", 27,
        "TeleportPortal", 28,
        "GreenRing", 29,
        "Collectible", 30,
        "UserCoin", 31,
        "DropRing", 32,
        "SpiderPortal", 33,
        "RedJumpPad", 34,
        "RedJumpRing", 35,
        "CustomRing", 36,
        "DashRing", 37,
        "GravityDashRing", 38,
        "CollisionObject", 39,
        "Special", 40,
        "SwingPortal", 41,
        "GravityTogglePortal", 42,
        "SpiderOrb", 43,
        "SpiderPad", 44,
        "TeleportOrb", 46,
        "AnimatedHazard", 47
    );
}

void LGameObject::registerGameObject(sol::state& lua)
{
    sol::usertype<GameObject> type = lua.new_usertype<GameObject>("GameObject", sol::no_constructor);
    
    type.set("x", sol::property(&GameObject::getPositionX, moveObjectX));
    type.set("y", sol::property(&GameObject::getPositionY, moveObjectY));


    type.set("rotation", sol::property(&GameObject::getRotation, &GameObject::setRotation));
    type.set("scale", sol::property(&GameObject::setScale, &GameObject::getScale));

    type.set("groupCount", sol::readonly_property(&GameObject::m_groupCount));
    type.set("type", sol::readonly_property(&GameObject::m_objectType));
    type.set("id", sol::readonly_property(&GameObject::m_objectID));

    type.set_function("addGroups", addGroup);
    type.set_function("setGroups", setGroups);
    type.set_function("removeGroups", removeGroups);
    type.set_function("getGroups", getGroups);
    type.set_function("removeAllGroups", removeAllGroups);
    type.set_function("forAllGroups", forAllGroups);
    type.set_function("getProperties", getProperties);

    type.set_function("getSaveString", [](GameObject* obj) -> std::string { return std::string(obj->getSaveString(LuaHelper::editor())); });

}