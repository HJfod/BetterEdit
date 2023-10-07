#include <Geode/DefaultInclude.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

static constexpr const int ZOOMLABEL_TAG = 6976;
bool viewMode = false;

// TODO: under construction

/*

class $modify(ViewEditorUI, EditorUI) {
    CCLabelBMFont* viewOnlyLabel;
    CCMenuItemSpriteExtra* toggleBtn;
    CCMenuItemSpriteExtra* backButton;

    void onToggleShowUI(CCObject* target) {
        viewMode = !viewMode;
        if (viewMode) {
            this->getChildByID("build-tabs-menu")->setVisible(false);
            this->getChildByID("playtest-menu")->setVisible(false);
            this->showUI(false);
            m_fields->viewOnlyLabel->setVisible(true);
            m_fields->backButton->setVisible(true);
            m_fields->toggleBtn->setVisible(false);
        } else {
            this->getChildByID("build-tabs-menu")->setVisible(true);
            this->getChildByID("playtest-menu")->setVisible(true);
            this->showUI(true);
            m_fields->viewOnlyLabel->setVisible(false);
            m_fields->backButton->setVisible(false);
            m_fields->toggleBtn->setVisible(true);
        }
    }

    bool init(LevelEditorLayer* lel) {
        if (EditorUI::init(lel)) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto spr = CircleButtonSprite::createWithSpriteFrameName("view-only.png"_spr);

        m_fields->toggleBtn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ViewEditorUI::onToggleShowUI));
        m_fields->toggleBtn->setContentSize(m_fields->toggleBtn->getScaledContentSize());
        m_fields->toggleBtn->setScale(.6f);
        // todo: STRING IDS
        m_swipeBtn->getParent()->addChild(m_fields->toggleBtn);

        m_fields->viewOnlyLabel = CCLabelBMFont::create("View-Only Mode", "bigFont.fnt");
        m_fields->viewOnlyLabel->setScale(.4f);
        m_fields->viewOnlyLabel->setOpacity(90);
        m_fields->viewOnlyLabel->setPosition(winSize.width / 2, winSize.height - 30.0f);
        m_fields->viewOnlyLabel->setVisible(false);
        this->addChild(m_fields->viewOnlyLabel);

        CCNode* backSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        backSprite->setScale(.7f);

        m_fields->backButton = CCMenuItemSpriteExtra::create(backSprite, this, menu_selector(ViewEditorUI::onToggleShowUI));
        m_fields->backButton->setPosition(
            m_playbackBtn->getPositionX(),
            m_trashBtn->getPositionY()
        );
        m_fields->backButton->setVisible(false);

        m_playbackBtn->getParent()->addChild(m_fields->backButton);

        return true;
    }
};

*/