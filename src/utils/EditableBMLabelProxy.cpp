#include "EditableBMLabelProxy.hpp"

EditableBMLabelProxy* EditableBMLabelProxy::create() {
    auto ret = new EditableBMLabelProxy();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

EditableBMLabelProxy* EditableBMLabelProxy::replace(
    CCLabelBMFont* existing,
    float width, std::string const& placeholder,
    std::function<void(std::string const&)> setValue
) {
    auto proxy = EditableBMLabelProxy::create();

    proxy->m_setValue = setValue;
    proxy->m_input = InputNode::create(width, placeholder.c_str());
    proxy->m_input->ignoreAnchorPointForPosition(false);
    proxy->m_input->getInput()->setDelegate(proxy);
    // 2021 Fod was sadistic for not doing this
    proxy->m_input->setContentSize(proxy->m_input->getBG()->getScaledContentSize());
    existing->getParent()->addChild(proxy->m_input);

    proxy->setString(existing->getString());
    proxy->setScale(existing->getScale());
    proxy->setZOrder(existing->getZOrder());
    proxy->setPosition(existing->getPosition());
    proxy->setAnchorPoint(existing->getAnchorPoint());
    proxy->setID(existing->getID());
    proxy->setTag(existing->getTag());

    existing->getParent()->addChild(proxy);
    existing->removeFromParent();
    
    return proxy;
}

void EditableBMLabelProxy::updateLabel() {}

void EditableBMLabelProxy::setString(const char* str) {
    this->setString(str, true);
}

void EditableBMLabelProxy::setString(const char* str, bool needUpdateLabel) {
    CCLabelBMFont::setString(str, needUpdateLabel);
    if (m_input && !m_ignoreLabelUpdate) {
        m_ignoreLabelUpdate = true;
        m_input->setString(str);
        m_ignoreLabelUpdate = false;
    }
}

void EditableBMLabelProxy::textChanged(CCTextInputNode*) {
    if (m_setValue && !m_ignoreLabelUpdate) {
        m_ignoreLabelUpdate = true;
        m_setValue(m_input->getString());
        m_ignoreLabelUpdate = false;
    }
}
