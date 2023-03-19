#include "Clip.hpp"

bool Clip::init(bool invert) {
    m_stencil = CCLayerColor::create({ 255, 255, 255, 255 });

    if (!CCClippingNode::init(m_stencil))
        return false;
    
    this->setInverted(invert);
    this->updateClippingRect();

    return true;
}

void Clip::updateClippingRect() {
    m_stencil->setContentSize(m_obContentSize);
}

Clip* Clip::create(bool invert) {
    auto ret = new Clip();
    if (ret && ret->init(invert)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void Clip::setContentSize(CCSize const& size) {
    CCClippingNode::setContentSize(size);
    this->updateClippingRect();
}
