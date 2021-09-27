#include "gridButton.hpp"
#include "../../BetterEdit.hpp"
#include <InputNode.hpp>
#include <string>
#include <sstream>
#include "../CustomKeybinds/loadEditorKeybindIndicators.hpp"

using namespace gd;
using namespace gdmake;
using namespace gdmake::extra;
using namespace cocos2d;

static constexpr const int ZOOMIN_TAG = 8001;
static constexpr const int ZOOMOUT_TAG = 8002;

bool isFloat(std::string const& myString) {
    std::istringstream iss(myString);
    float f;
    iss >> f;
    return iss.eof() && !iss.fail(); 
}

CCPoint getGridButtonPosition(EditorUI* self, int which) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto ratio = winSize.width / winSize.height;

    auto pOptionsBtn = getChild<CCNode*>(self->m_pCopyBtn->getParent(), 1);

    if (ratio > 1.6f)
        return {
            pOptionsBtn->getPositionX() - 55.0f - which * 18.0f,
            pOptionsBtn->getPositionY()
        };
    
    return {
        self->m_pLinkBtn->getPositionX() + 35.0f,
        (self->m_pLinkBtn->getPositionY() + self->m_pUnlinkBtn->getPositionY()) / 2.0f + which * 18.0f
    };
}

CCMenu* getGridButtonParent(EditorUI* self) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto ratio = winSize.width / winSize.height;

    if (ratio > 1.6f)
        return as<CCMenu*>(self->m_pCopyBtn->getParent());
    
    return as<CCMenu*>(self->m_pLinkBtn->getParent());
}

void zoomEditorGrid(EditorUI* ui, bool zoomIn) {
    auto size = BetterEdit::sharedState()->getGridSize();
    if (zoomIn)
        size /= 2;
    else
        size *= 2;

    BetterEdit::sharedState()->setGridSize(size);
    if (!BetterEdit::sharedState()->getAlwaysUseCustomGridSize())
        BetterEdit::sharedState()->setGridSizeEnabled(size != 30.0f);
    else
        BetterEdit::sharedState()->setGridSizeEnabled(true);
    
    if (BetterEdit::getShowGridOnChange()) {
        GameManager::sharedState()->setGameVariable("0038", true);
        ui->m_pEditorLayer->updateOptions();
    }

    ui->updateGridNodeSize();

    CATCH_NULL(as<CCTextInputNode*>(getGridButtonParent(ui)->getChildByTag(ZOOMINPUT_TAG)))
        ->setString(BetterEdit::sharedState()->getGridSizeAsString().c_str());
}

class EditorUI_CB : public EditorUI {
    public:
        void zoomGrid(CCObject* pSender) {
            zoomEditorGrid(this, bool_cast(as<CCNode*>(pSender)->getUserData()));
        }
};

class GridInputDelegate : public CCNode, public TextInputDelegate {
    public:
        void textChanged(CCTextInputNode* input) override {
            auto ui = LevelEditorLayer::get()->getEditorUI();
            auto size = BetterEdit::getGridSize();

            if (input->getString() && strlen(input->getString()))
                try {
                    size = std::stof(input->getString());
                    if (!isFloat(input->getString())) {
                        input->setString(BetterEdit::formatToString(size).c_str());
                        input->getTextField()->detachWithIME();
                    }
                } catch (...) {
                    input->setString(BetterEdit::formatToString(size).c_str());
                    input->getTextField()->detachWithIME();
                }

            BetterEdit::sharedState()->setGridSize(size);
            if (!BetterEdit::sharedState()->getAlwaysUseCustomGridSize())
                BetterEdit::sharedState()->setGridSizeEnabled(size != 30.0f);
            else
                BetterEdit::sharedState()->setGridSizeEnabled(true);

            GameManager::sharedState()->setGameVariable("0038", true);
            ui->m_pEditorLayer->updateOptions();

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

void showGridButtons(EditorUI* self, bool show) {
    CATCH_NULL(getGridButtonParent(self)->getChildByTag(ZOOMIN_TAG))->setVisible(show);
    CATCH_NULL(getGridButtonParent(self)->getChildByTag(ZOOMOUT_TAG))->setVisible(show);
    CATCH_NULL(getGridButtonParent(self)->getChildByTag(ZOOMINPUT_TAG))->setVisible(show);
}

void loadGridButtons(EditorUI* self) {
    auto d = GridInputDelegate::create();
    self->addChild(d);

    getGridButtonParent(self)->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(
                CCMenuItemSpriteExtra::create(
                    CCNodeConstructor()
                        .fromFrameName("GJ_zoomInBtn_001.png")
                        .scale(.35f)
                        .done(),
                    self,
                    (SEL_MenuHandler)&EditorUI_CB::zoomGrid
                )
            )
            .udata(1)
            .exec([self](auto t) -> void {
                addKeybindIndicator(self, t, "betteredit.increase_grid_size");
            })
            .tag(ZOOMIN_TAG)
            .move(getGridButtonPosition(self, 1))
            .done()
    );
    getGridButtonParent(self)->addChild(
        CCNodeConstructor<CCTextInputNode*>()
            .fromNode(CCTextInputNode::create("30", self, "bigFont.fnt", 30.0f, 30.0f))
            .scale(.5f)
            .text(BetterEdit::getGridSizeAsString().c_str())
            .exec([d](CCTextInputNode* i) -> void {
                i->setAllowedChars(inputf_NumeralFloat);
                i->setAnchorPoint({ 0, 0 });
                i->setLabelPlaceholderColor({ 150, 150, 150 });
                i->setDelegate(d);
            })
            .tag(ZOOMINPUT_TAG)
            .move(getGridButtonPosition(self, 0))
            .done()
    );
    getGridButtonParent(self)->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(
                CCMenuItemSpriteExtra::create(
                    CCNodeConstructor()
                        .fromFrameName("GJ_zoomOutBtn_001.png")
                        .scale(.35f)
                        .done(),
                    self,
                    (SEL_MenuHandler)&EditorUI_CB::zoomGrid
                )
            )
            .exec([self](auto t) -> void {
                addKeybindIndicator(self, t, "betteredit.decrease_grid_size");
            })
            .udata(0)
            .tag(ZOOMOUT_TAG)
            .move(getGridButtonPosition(self, -1))
            .done()
    );
}
