#include <Geode/modify/CreateParticlePopup.hpp>

using namespace geode::prelude;

class $modify(CopyParticleStringPopup, CreateParticlePopup) {
    $override
    bool init(ParticleGameObject* obj, CCArray* objs, gd::string str) {
        if (!CreateParticlePopup::init(obj, objs, str))
            return false;
        
        if (!Mod::get()->template getSettingValue<bool>("dev-mode")) {
            return true;
        }
        
        auto menu = CCMenu::create();
        menu->setID("side-menu"_spr);
        menu->setPosition(m_obContentSize.width / 2 - 100, 30);

        if (obj) {
            auto copySpr = ButtonSprite::create("CS", "bigFont.fnt", "GJ_button_05.png", .8f);
            auto copyBtn = CCMenuItemSpriteExtra::create(
                copySpr, this, menu_selector(CopyParticleStringPopup::onCopyString)
            );
            menu->addChild(copyBtn);

            auto pasteSpr = ButtonSprite::create("PS", "bigFont.fnt", "GJ_button_05.png", .8f);
            auto pasteBtn = CCMenuItemSpriteExtra::create(
                pasteSpr, this, menu_selector(CopyParticleStringPopup::onPasteString)
            );
            menu->addChild(pasteBtn);
        }

        menu->setLayout(RowLayout::create()->setDefaultScaleLimits(.1f, .3f));
        m_mainLayer->addChild(menu);

        handleTouchPriority(this);
        
        return true;
    }

    void onCopyString(CCObject*) {
        if (m_targetObject) {
            if (clipboard::write(GameToolbox::saveParticleToString(m_particle))) {
                Notification::create(
                    "Copied Particle String to Clipboard",
                    NotificationIcon::Success
                )->show();
            }
            else {
                Notification::create(
                    "Unable to copy Particle String",
                    NotificationIcon::Error
                )->show();
            }
        }
    }
    void onPasteString(CCObject*) {
        if (auto obj = m_targetObject) {
            this->onClose(nullptr);
            obj->setParticleString(clipboard::read());
            CreateParticlePopup::create(obj, nullptr)->show();
            Notification::create(
                "Pasted Particle String from Clipboard",
                NotificationIcon::Success
            )->show();
        }
    }
};
