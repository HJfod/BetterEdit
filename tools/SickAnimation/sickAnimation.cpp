#include "sickAnimation.hpp"

void doTheSickAnimation(LevelEditorLayer* lel) {
    // auto last = lel->getLastObjectX();

    // auto screenRight = lel->getObjectLayer()->convertToNodeSpace(
    //     { CCDirector::sharedDirector()->getScreenRight() + 50.0f, 0.0f }
    // );

    // CCARRAY_FOREACH_B_TYPE(lel->getAllObjects(), obj, GameObject) {
    //     auto time = obj->getPositionX() / last * 100.0f + 1.0f;
    //     if (obj->getPositionX() > screenRight.x)
    //         time = 0.5f;
    //     auto scale = obj->getScale();

    //     obj->setScale(0.0f);
    //     obj->runAction(CCSequence::create(CCArray::create(
    //         CCDelayTime::create(time),
    //         CCSpawn::create(
    //             CCEaseBackOut::create(
    //                 CCScaleTo::create(1.0f, scale)
    //             ),
    //             nullptr
    //         ),
    //         nullptr
    //     )));
    // }
    
    // lel->m_pEditorUI->showUI(false);
    // lel->m_pEditorUI->setVisible(false);
}
