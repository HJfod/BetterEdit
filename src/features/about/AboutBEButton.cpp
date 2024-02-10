
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include "AboutBEPopup.hpp"

using namespace geode::prelude;

class $modify(AboutBEPauseLayer, EditorPauseLayer) {
    $override
    bool init(LevelEditorLayer* lel) {
        if (!EditorPauseLayer::init(lel))
            return false;

        auto menu = this->getChildByID("guidelines-menu");

        auto spr = CCSprite::createWithSpriteFrameName("be-button.png"_spr);
        spr->setScale(.92f);
        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(AboutBEPauseLayer::onAbout)
        );
        btn->setID("about"_spr);
        menu->addChild(btn);
        menu->updateLayout();

        if (!Mod::get()->template getSavedValue<bool>("highlighted-be-button")) {
            auto overlay = CCLayerColor::create({ 0, 0, 0, 150 });
            overlay->setOpacity(0);
            overlay->runAction(CCFadeTo::create(.25f, 185));
            overlay->setZOrder(500);
            overlay->setID("be-highlight-overlay"_spr);
            this->addChild(overlay);

            btn->setPosition(btn->getParent()->convertToWorldSpace(btn->getPosition()));
            {
                auto ref = Ref(btn);
                btn->removeFromParent();
                overlay->addChild(btn);
            }

            btn->runAction(CCSequence::create(
                CCDelayTime::create(.1f),
                CCEaseElasticOut::create(CCScaleTo::create(.75f, 2.f), 2.f),
                CCTintTo::create(.15f, 0, 155, 255),
                CCTintTo::create(.15f, 255, 255, 255),
                CCDelayTime::create(.1f),
                CCTintTo::create(.15f, 0, 155, 255),
                CCTintTo::create(.15f, 255, 255, 255),
                CCEaseElasticIn::create(CCScaleTo::create(.75f, .92f), 2.f),
                CCCallFunc::create(this, callfunc_selector(AboutBEPauseLayer::markButtonAsHighlighted)),
                nullptr
            ));
        }
        
        return true;
    }

    void markButtonAsHighlighted() {
        Mod::get()->setSavedValue("highlighted-be-button", true);
        auto overlay = this->getChildByID("be-highlight-overlay"_spr);
        {
            auto btn = overlay->getChildByID("about"_spr);
            auto menu = this->getChildByID("guidelines-menu");
            auto ref = Ref(btn);
            btn->removeFromParent();
            menu->addChild(btn);
            menu->updateLayout();
        }
        overlay->runAction(CCSequence::create(
            CCFadeTo::create(.25f, 0),
            CCRemoveSelf::create(),
            nullptr
        ));
    }

    void onAbout(CCObject*) {
        AboutBEPopup::create()->show();
    }
};
