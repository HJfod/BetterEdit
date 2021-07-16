#include "StartPosSelect.hpp"
#include <thread>

bool StartPosSelect::init() {
    if (!CCNode::init())
        return false;
    
    this->m_pSlider = Slider::create(this, nullptr, 1.0f);
    this->m_pSlider->m_pTouchLogic->m_pThumb->setVisible(false);
    this->m_pSlider->setValue(5.0f);
    this->addChild(this->m_pSlider);
    
    return true;
}

void StartPosSelect::loadStartPositions(GJGameLevel* level, std::function<void()> cb) {
    // load level start positions

    // cb();
}

StartPosSelect* StartPosSelect::create() {
    auto pRet = new StartPosSelect();

    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
