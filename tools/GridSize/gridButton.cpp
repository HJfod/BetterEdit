#include "gridButton.hpp"
#include "../../BetterEdit.hpp"

using namespace gd;
using namespace gdmake;
using namespace gdmake::extra;
using namespace cocos2d;

static constexpr const int ZOOMIN_TAG = 8001;
static constexpr const int ZOOMOUT_TAG = 8002;

class EditorUI_CB : public EditorUI {
    public:
        void zoomGrid(CCObject* pSender) {
            auto size = BetterEdit::sharedState()->getGridSize();
            if (as<int>(as<CCNode*>(pSender)->getUserData()))
                size /= 2;
            else
                size *= 2;

            BetterEdit::sharedState()->setGridSize(size);
            if (!BetterEdit::sharedState()->getAlwaysUseCustomGridSize())
                BetterEdit::sharedState()->setGridSizeEnabled(size != 30.0f);
            else
                BetterEdit::sharedState()->setGridSizeEnabled(true);
            
            GameManager::sharedState()->setGameVariable("0038", true);
            this->m_pEditorLayer->updateOptions();

            this->updateGridNodeSize();
        }
};

void showGridButtons(EditorUI* self, bool show) {
    CATCH_NULL(self->m_pButton4->getParent()->getChildByTag(ZOOMIN_TAG))->setVisible(show);
    CATCH_NULL(self->m_pButton4->getParent()->getChildByTag(ZOOMOUT_TAG))->setVisible(show);
}

void loadGridButtons(EditorUI* self) {
    self->m_pButton4->getParent()->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(
                CCMenuItemSpriteExtra::create(
                    CCNodeConstructor()
                        .fromFrameName("GJ_zoomInBtn_001.png")
                        .scale(.6f)
                        .done(),
                    self,
                    (SEL_MenuHandler)&EditorUI_CB::zoomGrid
                )
            )
            .udata(1)
            .tag(ZOOMIN_TAG)
            .move(175.0f, 140.0f)
            .done()
    );
    self->m_pButton4->getParent()->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(
                CCMenuItemSpriteExtra::create(
                    CCNodeConstructor()
                        .fromFrameName("GJ_zoomOutBtn_001.png")
                        .scale(.6f)
                        .done(),
                    self,
                    (SEL_MenuHandler)&EditorUI_CB::zoomGrid
                )
            )
            .udata(0)
            .tag(ZOOMOUT_TAG)
            .move(145.0f, 140.0f)
            .done()
    );
}
