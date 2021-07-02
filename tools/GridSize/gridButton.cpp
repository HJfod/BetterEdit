#include "gridButton.hpp"
#include "../../BetterEdit.hpp"

using namespace gd;
using namespace gdmake;
using namespace gdmake::extra;
using namespace cocos2d;

class EditorUI_CB : public EditorUI {
    public:
        void zoomGrid(CCObject* pSender) {
            auto size = BetterEdit::sharedState()->getGridSize();
            if (as<int>(as<CCNode*>(pSender)->getUserData()))
                size /= 2;
            else
                size *= 2;

            BetterEdit::sharedState()->setGridSize(size);
            BetterEdit::sharedState()->setGridSizeEnabled(size != 30.0f);

            this->updateGridNodeSize();
        }
};

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
            .move(145.0f, 140.0f)
            .done()
    );
}
