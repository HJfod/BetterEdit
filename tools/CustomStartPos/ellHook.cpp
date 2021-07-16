#include "ellHook.hpp"
#include "StartPosSelect.hpp"

class EditLevelLayer_CB : public EditLevelLayer {
    public:
        void onStartPos(CCObject* pSender) {
            auto c = LoadingCircle::create();

            auto btn = as<CCMenuItemSpriteExtra*>(pSender);

            c->setScale(.5f);
            c->setPositionY(- (btn->getParent()->getPositionY() + btn->getPositionY()));
            c->show();

            btn->setEnabled(false);

            auto sel = StartPosSelect::create();
            sel->setScale(0.0f);
            sel->setPosition(btn->getPosition());

            sel->loadStartPositions(this->m_pLevel, [this, btn, sel, c]() -> void {
                c->fadeAndRemove();

                btn->getParent()->addChild(sel);

                btn->runAction(
                    CCSpawn::create(
                        CCEaseElasticOut::create(
                            CCMoveBy::create(1.0f, { -130.0f, 0.0f }), 1.0f
                        ),
                        CCEaseElasticOut::create(
                            CCRotateBy::create(1.0f, 360.0f), 1.0f
                        ),
                        nullptr
                    )
                );

                sel->runAction(
                    CCSequence::create(
                        CCDelayTime::create(0.3f),
                        CCEaseElasticOut::create(
                            CCScaleTo::create(1.0f, 1.0f), 1.0f
                        ),
                        nullptr
                    )
                );
            });
        }
};

void loadStartPosButton(EditLevelLayer* self, GJGameLevel* level) {
    // auto playBtn = getChild<CCMenuItemSpriteExtra*>(self->m_pButtonMenu, 1);

    // self->m_pButtonMenu->addChild(
    //     CCNodeConstructor<CCMenuItemSpriteExtra*>()
    //         .fromNode(CCMenuItemSpriteExtra::create(
    //             CCNodeConstructor()
    //                 .fromFrameName("GJ_playEditorBtn_001.png")
    //                 .scale(.7f)
    //                 .done(),
    //             self,
    //             (SEL_MenuHandler)&EditLevelLayer_CB::onStartPos
    //         ))
    //         .move(playBtn->getPosition() + CCPoint { 0.0f, -55.0f })
    //         .done()
    // );
}
