#include "pauseMenuHook.hpp"
#include "GoLiveLayer.hpp"
#include "../../BetterEdit.hpp"

using namespace gd;
using namespace gdmake;
using namespace gdmake::extra;
using namespace cocos2d;

class EditorPauseLayer_CB : public EditorPauseLayer {
    public:
        void onGoLive(CCObject*) {
            GoLiveLayer::create()->show();
        }
};

void loadLiveButton(EditorPauseLayer* self) {
    // auto winSize = CCDirector::sharedDirector()->getWinSize();
    // auto menu = as<CCMenu*>(self->m_pButton0->getParent());

    // auto btn = CCMenuItemSpriteExtra::create(
    //     CCNodeConstructor()
    //         .fromNode(ButtonSprite::create(
    //             "Go Live", 0, 0, "goldFont.fnt", "GJ_button_01.png", 0, .8f
    //         ))
    //         .scale(.8f)
    //         .done(),
    //     self,
    //     (SEL_MenuHandler)&EditorPauseLayer_CB::onGoLive
    // );
    // btn->setPosition(0, winSize.height - 60);
    // menu->addChild(btn);
}

