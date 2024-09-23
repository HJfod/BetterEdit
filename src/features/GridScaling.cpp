#include "GridScaling.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/ObjectToolbox.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/ui/TextInput.hpp>
#include <utils/Editor.hpp>

using namespace geode::prelude;

#ifndef GEODE_IS_ANDROID32

class $modify(ObjectToolbox) {
    $override
    float gridNodeSizeForKey(int id) {
        auto size = Mod::get()->template getSavedValue<float>("grid-size");
        if (size < 1 || roundf(size) == 30) {
            return ObjectToolbox::gridNodeSizeForKey(id);
        }
        return size;
    }
};

class $modify(GridUI, EditorUI) {
    struct Fields {
        OnUIHide onUIHide;
    };

    $override
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;
        
        if (!Mod::get()->template getSettingValue<bool>("grid-size-controls")) {
            return true;
        }
 
        // Move position slider a bit out of the way to make space

        auto container = CCMenu::create();
        container->setContentSize({ 80, 35 });
        container->ignoreAnchorPointForPosition(false);
        container->setAnchorPoint({ .5f, .5f });

        auto decSpr = CCSprite::createWithSpriteFrameName("GJ_zoomInBtn_001.png");
        decSpr->setScale(.4f);
        auto decBtn = CCMenuItemSpriteExtra::create(
            decSpr, this, menu_selector(GridUI::onGridSize)
        );
        decBtn->setTag(-1);
        container->addChildAtPosition(decBtn, Anchor::Left, ccp(15, 0));

        auto incSpr = CCSprite::createWithSpriteFrameName("GJ_zoomOutBtn_001.png");
        incSpr->setScale(.4f);
        auto incBtn = CCMenuItemSpriteExtra::create(
            incSpr, this, menu_selector(GridUI::onGridSize)
        );
        incBtn->setTag(1);
        container->addChildAtPosition(incBtn, Anchor::Right, ccp(-15, 0));

        auto input = TextInput::create(60.f, "Grid");
        input->setCommonFilter(CommonFilter::Float);
        input->setCallback([this](std::string const& num) {
            if (auto value = numFromString<float>(num)) {
                this->updateGridConstSize(value.unwrap(), false);
            }
        });
        input->setScale(.55f);
        input->setID("grid-size-input"_spr);
        container->addChildAtPosition(input, Anchor::Center);

        container->setID("grid-size-controls"_spr);
        
        // Handle other aspect ratios when there's not enough space next to the slider
        auto offset = ccp(165, -20);
        if (CCDirector::get()->getWinSize().aspect() <= 1.6f) {
            offset = ccp(0, -50);
        }
        this->addChildAtPosition(container, Anchor::Top, offset, false);

        m_fields->onUIHide.setFilter(UIShowFilter(this));
        m_fields->onUIHide.bind([container](auto* ev) {
            container->setVisible(ev->show);
        });

        this->updateGridConstSize();

        return true;
    }

    void updateGridConstSize(float nuevoValue = 0, bool updateInput = true) {
        if (nuevoValue < .9f || nuevoValue > 120.1f) {
            nuevoValue = 30;
        }
        Mod::get()->setSavedValue("grid-size", nuevoValue);
        this->updateGridNodeSize();
        if (updateInput) {
            static_cast<TextInput*>(
                this->getChildByID("grid-size-controls"_spr)
                    ->getChildByID("grid-size-input"_spr)
            )->setString(numToString(nuevoValue));
        }
        // Show grid if the size is not default
        if (roundf(nuevoValue) != 30 && Mod::get()->template getSettingValue<bool>("show-grid-on-size-change")) {
            GameManager::sharedState()->setGameVariable("0038", true);
            m_editorLayer->updateOptions();
        }
    }

    void onGridSize(CCObject* sender) {
        if (sender->getTag() == -1) {
            decrementGridSize(this);
        }
        else {
            incrementGridSize(this);
        }
    }

    $override
    void updateGridNodeSize() {
        auto size = Mod::get()->template getSavedValue<float>("grid-size");
        if (size < 1 || roundf(size) == 30) {
            return EditorUI::updateGridNodeSize();
        }

        // Pretend that the current mode is Create for the duration of the 
        // call to the original 
        auto actualMode = m_selectedMode;
        m_selectedMode = 2;
        EditorUI::updateGridNodeSize();
        m_selectedMode = actualMode;
    }
};

static std::array SNAP_GRID_SIZES {
    3.75f, 7.5f, 15.f, 30.f, 60.f, 90.f, 120.f
};
void decrementGridSize(EditorUI* ui) {
    auto value = Mod::get()->template getSavedValue<float>("grid-size");
    auto next = std::lower_bound(SNAP_GRID_SIZES.begin(), SNAP_GRID_SIZES.end(), value);
    if (next != SNAP_GRID_SIZES.begin()) {
        next--;
    }
    value = *next;
    static_cast<GridUI*>(ui)->updateGridConstSize(value);
}
void incrementGridSize(EditorUI* ui) {
    auto value = Mod::get()->template getSavedValue<float>("grid-size");
    auto next = std::upper_bound(SNAP_GRID_SIZES.begin(), SNAP_GRID_SIZES.end(), value);
    if (next == SNAP_GRID_SIZES.end()) {
        next--;
    }
    value = *next;
    static_cast<GridUI*>(ui)->updateGridConstSize(value);
}

#endif
