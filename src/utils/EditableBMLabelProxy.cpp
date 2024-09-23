#include "EditableBMLabelProxy.hpp"

EditableBMLabelProxy* EditableBMLabelProxy::create() {
    auto ret = new EditableBMLabelProxy();
    if (ret && ret->init()) {
        ret->scheduleUpdate();
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

EditableBMLabelProxy* EditableBMLabelProxy::replace(
    CCLabelBMFont* existing,
    CCNode* inputParent,
    float width, std::string const& placeholder,
    std::function<void(std::string const&)> onSetValue,
    std::function<void(std::string const&)> onUpdate
) {
    auto proxy = EditableBMLabelProxy::create();

    proxy->m_onSetValue = onSetValue;
    proxy->m_onUpdate = onUpdate;
    proxy->m_inputParent = inputParent;
    proxy->m_input = TextInput::create(width, placeholder.c_str());
    proxy->m_input->ignoreAnchorPointForPosition(false);
    proxy->m_input->setDelegate(proxy);
    proxy->m_inputParent->addChild(proxy->m_input);

    auto parent = existing->getParent();
    parent->insertBefore(proxy, existing);

    proxy->setString(existing->getString());
    proxy->setScale(existing->getScale());
    proxy->setZOrder(existing->getZOrder());
    proxy->setOrderOfArrival(existing->getOrderOfArrival());
    proxy->setPosition(existing->getPosition());
    proxy->setAnchorPoint(existing->getAnchorPoint());
    proxy->setID(existing->getID());
    proxy->setTag(existing->getTag());
    proxy->setVisible(existing->isVisible());

    existing->removeFromParent();
    
    return proxy;
}

static float getWorldScale(CCNode* node) {
    if (auto parent = node->getParent()) {
        return getWorldScale(parent) * node->getScale();
    }
    return node->getScale();
}

void EditableBMLabelProxy::update(float) {
    if (m_input) {
        m_input->setPosition(
            m_inputParent->convertToNodeSpace(this->getParent()->convertToWorldSpace(m_obPosition))
        );
        m_input->setScale(getWorldScale(this) * 1.15f);
    }
}

void EditableBMLabelProxy::setPosition(CCPoint const& pos) {
    CCLabelBMFont::setPosition(pos);
    if (m_input) m_input->setPosition(
        m_inputParent->convertToNodeSpace(this->getParent()->convertToWorldSpace(m_obPosition))
    );
}

void EditableBMLabelProxy::setScale(float scale) {
    CCLabelBMFont::setScale(scale);
    if (m_input) m_input->setScale(scale * 1.15f);
}

void EditableBMLabelProxy::setColor(ccColor3B const& color) {
    CCLabelBMFont::setColor(color);
    if (m_input) m_input->getInputNode()->setLabelNormalColor(color);
}

void EditableBMLabelProxy::updateLabel() {
    if (m_input) {
        this->setContentSize(m_input->getScaledContentSize());
    }
}

void EditableBMLabelProxy::setString(const char* str) {
    this->setString(str, true);
}

void EditableBMLabelProxy::setString(const char* str, bool needUpdateLabel) {
    CCLabelBMFont::setString(str, needUpdateLabel);
    if (m_input && !m_ignoreLabelUpdate) {
        m_ignoreLabelUpdate = true;
        m_input->setString(str);
        if (m_onUpdate) {
            m_onUpdate(str);
        }
        m_ignoreLabelUpdate = false;
    }
}

void EditableBMLabelProxy::textChanged(CCTextInputNode*) {
    if (m_onSetValue && !m_ignoreLabelUpdate) {
        m_ignoreLabelUpdate = true;
        m_onSetValue(m_input->getString());
        m_ignoreLabelUpdate = false;
    }
}

TextInput* EditableBMLabelProxy::getInput() const {
    return m_input;
}
