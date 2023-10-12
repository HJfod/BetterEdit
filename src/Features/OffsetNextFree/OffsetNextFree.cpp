#include <Geode/Geode.hpp>
#include <Geode/modify/CustomizeObjectLayer.hpp>
#include <Geode/modify/SetGroupIDLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

#include "OffsetNextFreePopup.hpp"
#include "OffsetManager.hpp"

using namespace geode::prelude;

class $modify(LevelEditorLayer) {
    int getNextColorChannel() {
        std::array<bool, 1000> usedColors = this->getUsedColors();
        auto offset = OffsetManager::get()->m_colorOffset;
        for (int i = 1; i < 1000; i++) {
            if (!usedColors[i] && i >= offset) {
                return i;
            }
        }
        return 999;
    }

    int getNextFreeGroupID(CCArray* p0) {
        auto objects = this->getAllObjects();
        auto offset = OffsetManager::get()->m_groupOffset;
        std::array<bool, 1000> usedGroups = this->getUsedGroups(CCArrayExt<GameObject*>(objects));
        for (size_t i = 1; i < 1000; i++) {
            if (!usedGroups[i] && i >= offset) {
                return i;
            }
        }
        return 999;
    }

    std::array<bool, 1000> getUsedGroups(CCArrayExt<GameObject*> objects) {
        std::array<bool, 1000> ret;
        std::fill(ret.begin(), ret.end(), false);
        for (auto object : objects) {
            for (short id : object->getGroupIDs()) {
                ret[id] = true;
            }
        }

        return ret;
    }

    std::array<bool, 1000> getUsedColors() {
        std::array<bool, 1000> ret;
        std::fill(ret.begin(), ret.end(), false);
        for (auto object : CCArrayExt<GameObject*>(m_objects)) {
            if (object->m_baseColor) {
                int colorID = 0;
                if (object->m_baseColor->m_colorID == 0) {
                    colorID = object->m_baseColor->m_defaultColorID;
                } else {
                    colorID = object->m_baseColor->m_colorID;
                }

                if (colorID < 1000) {
                    ret[colorID] = true;
                }
            }

            if (object->m_detailColor) {
                int colorID = 0;
                if (object->m_detailColor->m_colorID == 0) {
                    colorID = object->m_detailColor->m_defaultColorID;
                } else {
                    colorID = object->m_detailColor->m_colorID;
                }

                if (colorID < 1000) {
                    ret[colorID] = true;
                }
            }
        }
        auto effectManager = GJEffectManager::get();
        for (auto colorAction : effectManager->m_colorActionsForColor) {
            if (!colorAction) {
                continue;
            }
            ccColor3B white = {255, 255, 255};
            bool isDefaultColor = colorAction->m_fromColor == white &&
                colorAction->m_targetColor == white &&
                colorAction->m_duration == 0;
            
            if (isDefaultColor || colorAction->m_colorID > 999) {
                continue;
            }

            ret[colorAction->m_colorID] = true;
        }

        return ret;
    }

    bool colorExists(int colorID) {
        bool ret = false;
        for (auto object : CCArrayExt<GameObject*>(m_objects)) {
            if (object->m_baseColor) {
                if (
                    (object->m_baseColor->m_colorID == 0 && 
                    object->m_baseColor->m_defaultColorID == colorID) ||
                    object->m_baseColor->m_colorID == colorID
                ) {
                    ret = true;
                    break;
                }
            }
            if (object->m_detailColor) {
                if (
                    (object->m_detailColor->m_colorID == 0 && 
                    object->m_detailColor->m_defaultColorID == colorID) ||
                    object->m_detailColor->m_colorID == colorID
                ) {
                    ret = true;
                    break;
                }
            }
        }
        return ret;
    }
};

class $modify(OffsetCustomizeObjectLayer, CustomizeObjectLayer) {
    bool init(GameObject* target, CCArray* targets) {
        if (!CustomizeObjectLayer::init(target, targets)) {
            return false;
        }
        auto offset = OffsetManager::get()->m_colorOffset;
        std::string sprite;
        if (offset == 0) {
            sprite = "GJ_button_04.png";
        } else {
            sprite = "GJ_button_02.png";
        }

        auto spr = ButtonSprite::create(std::to_string(offset).c_str(), 20, true, "goldFont.fnt", sprite.c_str(), 25, 0.6f);
        auto button = CCMenuItemSpriteExtra::create(
            spr,
            this,
            menu_selector(OffsetCustomizeObjectLayer::openOffsetPopup)
        );
        button->setID("offset-button"_spr);
        auto menu = static_cast<CCMenu*>(m_mainLayer->getChildByID("next-free-menu"));
        menu->addChild(button);
        menu->updateLayout();

        return true;
    }

    void openOffsetPopup(CCObject* sender) {
        OffsetNextFreePopup::create(OffsetType::Color, this)->show();
    }
};

class $modify(OffsetSetGroupIDLayer, SetGroupIDLayer) {
    bool init(GameObject* object, CCArray* objects) {
        if (!SetGroupIDLayer::init(object, objects)) {
            return false;
        }

        auto menu = getChildOfType<CCMenu>(this->m_mainLayer, 0);
        auto offset = OffsetManager::get()->m_groupOffset;
        std::string sprite;
        if (offset == 0) {
            sprite = "GJ_button_04.png";
        } else {
            sprite = "GJ_button_02.png";
        }

        auto spr = ButtonSprite::create(std::to_string(offset).c_str(), 20, true, "goldFont.fnt", sprite.c_str(), 25, 0.6f);
        auto button = CCMenuItemSpriteExtra::create(
            spr,
            this,
            menu_selector(OffsetSetGroupIDLayer::openOffsetPopup)
        );
        button->setID("offset-button"_spr);
        menu->addChild(button);
        button->setPosition(-85.f, 175.f);

        // This is some high level badassery
        for (auto object : CCArrayExt<CCNode*>(menu->getChildren())) {
            auto buttonSprite = getChildOfType<ButtonSprite>(object, 0);
            if (!buttonSprite) {
                continue;
            }

            auto label = getChildOfType<CCLabelBMFont>(buttonSprite, 0);
            if (!label) {
                continue;
            }

            std::string labelStr = label->getString();
            if (labelStr == "Next Free") {
                object->setPositionX(-140.f);
            }
        }

        return true;
    }

    void openOffsetPopup(CCObject* sender) {
        OffsetNextFreePopup::create(OffsetType::Group, this)->show();
    }
};