
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include "AboutBEPopup.hpp"
#include "SupportPopup.hpp"

using namespace geode::prelude;

class $modify(AboutBEPauseLayer, EditorPauseLayer) {
    $override
    bool init(LevelEditorLayer* lel) {
        if (!EditorPauseLayer::init(lel))
            return false;

        if (!Mod::get()->template getSavedValue<bool>("dont-show-support-button")) {
            if (auto topMenu = this->getChildByID("top-menu")) {
                auto supportSpr = CCSprite::createWithSpriteFrameName("support-btn.png"_spr);

                auto glow = CCSprite::createWithSpriteFrameName("support-btn-glow.png"_spr);
                glow->setZOrder(2);
                glow->setBlendFunc({ GL_SRC_ALPHA, GL_ONE });
                glow->setOpacity(125);
                glow->runAction(CCRepeat::create(
                    CCSequence::create(
                        CCEaseInOut::create(CCFadeTo::create(1.5f, 0), 4.f),
                        CCEaseInOut::create(CCFadeTo::create(1.5f, 125), 4.f),
                        nullptr
                    ),
                    1000
                ));
                supportSpr->addChildAtPosition(glow, Anchor::Center);

                auto particleFall = GameToolbox::particleFromString("37a-1a2a0.33a15a-90a92a0a0a59a0a0a-10a0a0a0a0a1a1a0a48a1a0a0a0a0.27451a0a1a0a0a1a0a165a1a0a0.686275a0a0.235294a0a1a0a0a0a0.61a0a0a0a0a0a0a0a0a2a1a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0", nullptr, false);
                particleFall->setZOrder(-1);
                particleFall->setScaleX(1.61f);
                particleFall->setScaleY(1.61f);
                supportSpr->addChildAtPosition(particleFall, Anchor::Bottom);

                auto particleTop1 = GameToolbox::particleFromString("5a-1a2a0.33a15a-90a180a0a0a59a0a0a-10a0a0a0a0a20a1a75a48a1a0a0a0a0.27451a0a1a0a2a1a0a165a1a0a0.686275a0a0.235294a0a1a0a0a0a0.61a0a21a8a29a22a-11a40a1a2a1a0a0a0a158a0a0a0a0a0a0a0a0a0a0a0a0a0a0", nullptr, false);
                particleTop1->setPosition({ supportSpr->getContentWidth() * .25f, supportSpr->getContentHeight() / 2 });
                particleTop1->setZOrder(1);
                particleTop1->setScaleX(.91f);
                particleTop1->setScaleY(.91f);
                particleTop1->setOpacity(105);
                supportSpr->addChild(particleTop1);

                auto particleTop2 = GameToolbox::particleFromString("5a-1a2a0.33a15a-90a180a0a0a59a0a0a-10a0a0a0a0a20a1a75a48a1a0a0a0a0.27451a0a1a0a2a1a0a165a1a0a0.686275a0a0.235294a0a1a0a0a0a0.61a0a21a8a29a22a-11a40a1a2a1a0a0a0a158a0a0a0a0a0a0a0a0a0a0a0a0a0a0", nullptr, false);
                particleTop2->setPosition({ supportSpr->getContentWidth() * .75f, supportSpr->getContentHeight() / 2 });
                particleTop2->setZOrder(1);
                particleTop2->setScaleX(.91f);
                particleTop2->setScaleY(.91f);
                particleTop2->setOpacity(105);
                supportSpr->addChild(particleTop2);

                supportSpr->setScale(.8f);
                supportSpr->runAction(CCRepeat::create(
                    CCSequence::create(
                        CCEaseInOut::create(CCScaleTo::create(1.5f, .7f), 4.f),
                        CCEaseInOut::create(CCScaleTo::create(1.5f, .8f), 4.f),
                        nullptr
                    ),
                    1000
                ));
                
                auto supportBtn = CCMenuItemSpriteExtra::create(
                    supportSpr, this, menu_selector(AboutBEPauseLayer::onSupport)
                );
                supportBtn->setID("support-be-btn"_spr);
                topMenu->addChild(supportBtn);
                topMenu->updateLayout();
            }
        }

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

    void onSupport(CCObject*) {
        SupportPopup::create(true)->show();
    }
    void onAbout(CCObject*) {
        AboutBEPopup::create()->show();
    }
};
