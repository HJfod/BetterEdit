#include "AutoLinkObject.hpp"
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/ObjectToolbox.hpp>
#include <Geode/binding/FLAlertLayer.hpp>
#include <Geode/binding/CreateMenuItem.hpp>
#include <Geode/utils/ranges.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/loader/ModEvent.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/GameObject.hpp>
#include <MoreTabs.hpp>
#include <other/Utils.hpp>
#include <tracking/Tracking.hpp>

using namespace geode::prelude;

static bool isAround(float a, float b) {
    return fabsf(a - b) < 2.f;
}

bool AutoLinkObject::init(Ref<AutoLinkSet> set) {
    m_eObjType = CCObjectType::GameObject;

    if (!CCSprite::init())
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

CCArray* AutoLinkObject::getObjects() const {
    return m_objects;
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
        def.set(
            isAround(obj->getPositionX() - this->getPositionX(), -30.f),
            isAround(obj->getPositionX() - this->getPositionX(), 30.f),
            isAround(obj->getPositionY() - this->getPositionY(), -30.f),
            isAround(obj->getPositionY() - this->getPositionY(), 30.f),
            obj->neighborage()
        );
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
        EditorUI::get()->repositionObjectsToCenter(m_objects, m_obPosition, true);
    }
}

struct $modify(AutoLinkUI, EditorUI) {
    Ref<AutoLinkSet> selected;
    int tag;

    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;
        
        auto btns = CCArray::create();
        for (auto& set : AutoLinkManager::get()->getSets()) {
            auto btn = this->getCreateBtn(AutoLinkObject::OBJ_ID, 4);
            btn->setUserObject(set);
            btns->addObject(btn);
        }
        btns->addObject(CCMenuItemSpriteExtra::create(
            createEditorButtonSprite("edit_addCBtn_001.png"),
            this, menu_selector(AutoLinkUI::onNewAutoLinkSet)
        ));
        m_fields->tag = MoreTabs::get(this)->addCreateTab("link-obj.png"_spr, btns);

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

    void selectObject(GameObject* obj, bool undo) {
        if (obj->m_objectID == AutoLinkObject::OBJ_ID) {
            this->selectObjects(
                static_cast<AutoLinkObject*>(obj)->getObjects(),
                undo
            );
            this->updateObjectInfoLabel();
        }
        else {
            EditorUI::selectObject(obj, undo);
        }
    }

    void moveObject(GameObject* obj, CCPoint delta) {
        if (obj->m_objectID == AutoLinkObject::OBJ_ID) {
            for (auto& sub : CCArrayExt<GameObject>(
                static_cast<AutoLinkObject*>(obj)->getObjects()
            )) {
                auto _ = BlockAll();
                EditorUI::moveObject(sub, delta);
            }
        }
        EditorUI::moveObject(obj, delta);
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

$on_mod(Loaded) {
    ObjectToolbox::sharedState()->addObject(AutoLinkObject::OBJ_ID, "link-obj.png"_spr);
}
