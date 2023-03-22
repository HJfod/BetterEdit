#include "AutoLinkObject.hpp"
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/ObjectToolbox.hpp>
#include <Geode/utils/ranges.hpp>
#include <Geode/loader/ModEvent.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/GameObject.hpp>
#include <MoreTabs.hpp>
#include <other/Utils.hpp>

static bool isAround(float a, float b) {
    return fabsf(a - b) < 2.f;
}

bool AutoLinkObject::init(Ref<AutoLinkSet> set) {
    m_eObjType = CCObjectType::GameObject;

    if (!CCSprite::initWithSpriteFrameName("link-obj.png"_spr))
        return false;
    
    m_textureName = "link-obj.png"_spr;
    m_objectID = AutoLinkObject::OBJ_ID;
    m_set = set;

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
    if (!m_set) return;
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
    ObjectToolbox::sharedState()->addObject(AutoLinkObject::OBJ_ID, "link-obj.png"_spr);
}

struct $modify(AutoLinkUI, EditorUI) {
    Ref<AutoLinkSet> selected;

    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;
        
        auto btns = CCArray::create();
        btns->addObject(this->getCreateBtn(AutoLinkObject::OBJ_ID, 4));
        for (auto& set : AutoLinkManager::get()->getSets()) {
            auto btn = this->getCreateBtn(AutoLinkObject::OBJ_ID, 4);
            btn->setUserObject(set);
            btns->addObject(btn);
        }
        btns->addObject(CCMenuItemSpriteExtra::create(
            createEditorButtonSprite("edit_addCBtn_001.png"),
            this, menu_selector(AutoLinkUI::onNewAutoLinkSet)
        ));
        MoreTabs::get(this)->addCreateTab("link-obj.png"_spr, btns);

        return true;
    }

    void onNewAutoLinkSet(CCObject*) {
        if (!m_selectedObjects || !m_selectedObjects->count()) {
            return FLAlertLayer::create(
                "Select Objects",
                "You need to select at least two objects to create an autolink set",
                "OK"
            )->show();
        }
        AutoLinkManager::get()->figureOutSet(m_selectedObjects);
    }

    void onCreateButton(CCObject* sender) {
        EditorUI::onCreateButton(sender);
        auto item = static_cast<CreateMenuItem*>(sender);
        if (item->m_objectID == AutoLinkObject::OBJ_ID) {
            m_fields->selected = static_cast<AutoLinkSet*>(item->getUserObject());
        }
        else {
            m_fields->selected = nullptr;
        }
    }
};

struct $modify(GameObject) {
    static GameObject* createWithKey(int key) {
        if (key == AutoLinkObject::OBJ_ID) {
            return AutoLinkObject::create(
                EditorUI::get() ? 
                    static_cast<AutoLinkUI*>(EditorUI::get())->m_fields->selected :
                    nullptr
            );
        }
        return GameObject::createWithKey(key);
    }
};
