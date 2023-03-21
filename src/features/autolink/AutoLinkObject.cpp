#include "AutoLinkObject.hpp"
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/ObjectToolbox.hpp>
#include <Geode/utils/ranges.hpp>
#include <Geode/loader/ModEvent.hpp>

static bool isAround(float a, float b) {
    return fabsf(a - b) < 2.f;
}

bool AutoLinkObject::init(Ref<AutoLinkSet> set) {
    m_eObjType = CCObjectType::GameObject;

    if (!CCSprite::initWithSpriteFrameName("link-obj"_spr))
        return false;
    
    m_textureName = "link-obj"_spr;
    m_objectID = AutoLinkObject::OBJ_ID;

    this->commonSetup();

    return true;
}

void AutoLinkObject::customObjectSetup(gd::map<gd::string, gd::string>& map) {
}

AutoLinkNeighbour AutoLinkObject::neighborage() const {
    return m_slope ? AutoLinkNeighbour::Slope : AutoLinkNeighbour::Block;
}

AutoLinkObject* AutoLinkObject::create(Ref<AutoLinkSet> set) {
    auto ret = new AutoLinkObject;
    if (ret && ret->init(set)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void AutoLinkObject::cleanAutoLinkObjects() {
    ranges::remove(s_autoLinkObjects, [](Ref<AutoLinkObject> obj) {
        return !obj->getParent();
    });
}

void AutoLinkObject::updateLinking() {
    this->cleanAutoLinkObjects();
    AutoLinkDefinition def;
    for (auto obj : s_autoLinkObjects) {
        bool left  = isAround(obj->getPositionX() - this->getPositionX(), -30.f);
        bool right = isAround(obj->getPositionX() - this->getPositionX(), 30.f);
        bool down  = isAround(obj->getPositionY() - this->getPositionY(), -30.f);
        bool up    = isAround(obj->getPositionY() - this->getPositionY(), 30.f);
        if (left && up)         def.defs.upLeft = obj->neighborage();
        else if (up)            def.defs.up = obj->neighborage();
        else if (up && right)   def.defs.upRight = obj->neighborage();
        else if (right)         def.defs.right = obj->neighborage();
        else if (down && right) def.defs.downRight = obj->neighborage();
        else if (down)          def.defs.down = obj->neighborage();
        else if (down && left)  def.defs.downLeft = obj->neighborage();
        else if (left)          def.defs.left = obj->neighborage();
    }
    auto str = m_set->getObject(def);
    if (m_objectString != str) {
        for (auto obj : CCArrayExt<GameObject>(m_objects)) {
            LevelEditorLayer::get()->removeObjectFromSection(obj);
            LevelEditorLayer::get()->removeSpecial(obj);
            EditorUI::get()->deselectObject(obj);
            obj->deactivateObject(true);
        }
        m_objectString = str;
        m_objects = LevelEditorLayer::get()->createObjectsFromString(str, true);
    }
}

void AutoLinkObject::setPosition(CCPoint const& pos) {
    GameObject::setPosition(pos);
    this->updateLinking();
}

$on_mod(Loaded) {
    ObjectToolbox::sharedState()->addObject(AutoLinkObject::OBJ_ID, "link-obj"_spr);
}
