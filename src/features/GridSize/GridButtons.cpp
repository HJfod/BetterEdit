#include <Geode/Geode.hpp>

#include <Geode/modify/EditorUI.hpp>

#include "other/KeybindUtils.hpp"
#include "GridSizeDelegate.hpp"

using namespace geode::prelude;

static constexpr const int ZOOMIN_TAG = 8001;
static constexpr const int ZOOMOUT_TAG = 8002;
static constexpr const int ZOOMINPUT_TAG = 8003;

bool isFloat(std::string const& myString) {
    std::istringstream iss(myString);
    float f;
    iss >> f;
    return iss.eof() && !iss.fail(); 
}

class GridInputDelegate : public CCNode, public TextInputDelegate {
    public:
        void textChanged(CCTextInputNode* input) override {
            auto ui = LevelEditorLayer::get()->m_editorUI;
            auto size = GridSizeDelegate::get()->getGridSize();

            if (input->getString() && strlen(input->getString()))
                try {
                    size = std::stof(input->getString());
                    if (!isFloat(input->getString())) {
                        input->setString(GridSizeDelegate::get()->getGridSizeString().c_str());
                        input->getTextField()->detachWithIME();
                    }
                } catch (...) {
                    input->setString(GridSizeDelegate::get()->getGridSizeString().c_str());
                    input->getTextField()->detachWithIME();
                }

            GridSizeDelegate::get()->setGridSize(size);

            ui->updateGridNodeSize();
        }

        static GridInputDelegate* create() {
            auto pRet = new GridInputDelegate();

            if (pRet && pRet->init()) {
                pRet->autorelease();
                return pRet;
            }

            CC_SAFE_DELETE(pRet);
            return nullptr;
        }
};

class $modify(EditorUI_CB, EditorUI) {
    CCPoint getGridButtonPosition(int which) {
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto ratio = winSize.width / winSize.height;

        auto pOptionsBtn = getChildByID("settings-menu")->getChildByID("settings-button");

        if (ratio > 1.6f)
            return {
                pOptionsBtn->getPositionX() - 55.0f - which * 18.0f,
                pOptionsBtn->getPositionY()
            };
        
        return {
            m_linkBtn->getPositionX() + 35.0f,
            (m_linkBtn->getPositionY() + m_unlinkBtn->getPositionY()) / 2.0f + which * 18.0f
        };
    }

    CCMenu* getGridButtonParent() {
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto ratio = winSize.width / winSize.height;

        if (ratio > 1.6f)
            return as<CCMenu*>(getChildByID("settings-menu"));
        
        return as<CCMenu*>(m_linkBtn->getParent());
    }

    void zoomEditorGrid(bool zoomIn) {
        auto size = GridSizeDelegate::get()->getGridSize();
        if (zoomIn)
            size /= 2;
        else
            size *= 2;

        GridSizeDelegate::get()->setGridSize(size);

        updateGridNodeSize();

        as<CCTextInputNode*>(getGridButtonParent()->getChildByTag(ZOOMINPUT_TAG))
            ->setString(GridSizeDelegate::get()->getGridSizeString().c_str());        
    }

    void zoomGrid(CCObject* pSender) {
        zoomEditorGrid(reinterpret_cast<bool>(as<CCNode*>(pSender)->getUserData()));
    }

    bool init(LevelEditorLayer* p0) {
        if (!EditorUI::init(p0))
            return false;

        auto d = GridInputDelegate::create();
        addChild(d);

        auto zoomInSprite = CCSprite::createWithSpriteFrameName("GJ_zoomInBtn_001.png");
        zoomInSprite->setScale(0.35f);

        auto zoomInButton = CCMenuItemSpriteExtra::create(
            zoomInSprite,
            this,
            (SEL_MenuHandler)&EditorUI_CB::zoomGrid
        );

        zoomInButton->setUserData(reinterpret_cast<void*>(1));
        zoomInButton->setTag(ZOOMIN_TAG);
        zoomInButton->setPosition(getGridButtonPosition(1));

        KEYBIND_FOR_BUTTON(zoomInButton, "decrease-grid-size"_spr)
        getGridButtonParent()->addChild(zoomInButton);

        auto zoomTextInput = CCTextInputNode::create(30.0f, 30.0f, "30", 30.0f, "bigFont.fnt");
        
        zoomTextInput->setScale(0.5f);
        zoomTextInput->setAllowedChars("0123456789.");
        zoomTextInput->setAnchorPoint({ 0, 0 });
        zoomTextInput->setLabelPlaceholderColor({ 150, 150, 150 });
        zoomTextInput->setString(GridSizeDelegate::get()->getGridSizeString().c_str());
        
        zoomTextInput->setDelegate(d);
        zoomTextInput->setTag(ZOOMINPUT_TAG);

        zoomTextInput->setPosition(getGridButtonPosition(0));

        getGridButtonParent()->addChild(zoomTextInput);

        auto zoomOutSprite = CCSprite::createWithSpriteFrameName("GJ_zoomOutBtn_001.png");
        zoomOutSprite->setScale(0.35f);

        auto zoomOutButton = CCMenuItemSpriteExtra::create(
            zoomOutSprite,
            this,
            (SEL_MenuHandler)&EditorUI_CB::zoomGrid
        );

        zoomOutButton->setUserData(reinterpret_cast<void*>(0));
        zoomOutButton->setTag(ZOOMOUT_TAG);
        zoomOutButton->setPosition(getGridButtonPosition(-1));

        KEYBIND_FOR_BUTTON(zoomOutButton, "increase-grid-size"_spr)
        getGridButtonParent()->addChild(zoomOutButton);

        return true;
    }
};

$execute {
    BindManager::get()->registerBindable(BindableAction(
        "increase-grid-size"_spr,
        "Increase Grid Size",
        "Increases the Grid Size",
        {},
        Category::EDITOR_UI
    ), "robtop.geometry-dash/zoom-out");

    BindManager::get()->registerBindable(BindableAction(
        "decrease-grid-size"_spr,
        "Decrease Grid Size",
        "Decreases the Grid Size",
        {},
        Category::EDITOR_UI
    ), "increase-grid-size"_spr);
}