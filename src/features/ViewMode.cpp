#include <Geode/DefaultInclude.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

static constexpr const int ZOOMLABEL_TAG = 6976;

/*class $modify(EditorUI) {
    bool init(LevelEditorLayer* lel) {
        if (EditorUI::init(lel)) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto currentZoomLabel = CCLabelBMFont::create("", "bigFont.fnt");
        currentZoomLabel->setScale(.5f);
        currentZoomLabel->setPosition(winSize.width / 2, winSize.height - 60.0f);
        currentZoomLabel->setTag(ZOOMLABEL_TAG);
        currentZoomLabel->setOpacity(0);
        currentZoomLabel->setZOrder(99999);
        this->addChild(currentZoomLabel);

        CCMenuItemSpriteExtra* toggleBtn;
        auto spr = CircleButtonSprite::createWithSpriteFrameName("view-only.png"_spr);

        toggleBtn = CCMenuItemSpriteExtra::create()

        this->m_swipeBtn->getParent()->addChild(
            CCNodeConstructor<CCMenuItemSpriteExtra*>()
                .fromNode(
                    CCMenuItemSpriteExtra::create(
                        CCNodeConstructor()
                            .fromBESprite("BE_eye-on-btn.png")
                            .scale(.6f)
                            .exec([](auto t) -> void {
                                t->setContentSize(t->getScaledContentSize());
                            })
                            .done(),
                        this,
                        (SEL_MenuHandler)&EditorUI_CB::onToggleShowUI
                    )
                )
                .exec([this](auto p) -> void {
                    addKeybindIndicator(this, p, "betteredit.toggle_ui");
                })
                .tag(TOGGLEUI_TAG)
                .move(getShowButtonPosition(this))
                .save(&toggleBtn)
                .done()
        );

        if (BetterEdit::isEditorViewOnlyMode()) {
            auto viewOnlyLabel = CCLabelBMFont::create("View-Only Mode", "bigFont.fnt");

            viewOnlyLabel->setScale(.4f);
            viewOnlyLabel->setOpacity(90);
            viewOnlyLabel->setPosition(winSize.width / 2, winSize.height - 30.0f);

            this->addChild(viewOnlyLabel);

            auto backButton = CCMenuItemSpriteExtra::create(
                CCNodeConstructor()
                    .fromFrameName("GJ_arrow_01_001.png")
                    .scale(.75f)
                    .done(),
                this,
                menu_selector(EditorUI_CB::onExitViewMode)
            );

            backButton->setPosition(
                this->m_playbackBtn->getPositionX(),
                this->m_pTrashBtn->getPositionY()
            );
            backButton->setTag(VIEWMODE_BACKBTN_TAG);

            this->m_playbackBtn->getParent()->addChild(backButton);

            this->showUI(false);
        }
    }
};*/