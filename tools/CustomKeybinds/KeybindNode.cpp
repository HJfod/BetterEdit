#include "KeybindNode.hpp"

CCArray* g_nodes = nullptr;

bool isNodeVisible(CCNode* t) {
    if (!t->isVisible())
        return false;
    
    if (t->getParent())
        return isNodeVisible(t->getParent());
    
    return true;
}

CCNode* KeybindNode::getTarget() {
    return this->m_pTarget;
}

bool KeybindNode::init(CCNode* t, const char* cb, bool addToList) {
    if (!this->CCSprite::init())
        return false;

    if (!g_nodes) {
        g_nodes = CCArray::create();
        g_nodes->retain();
    }

    this->m_pTarget = t;

    this->m_obTarget = KeybindManager::get()->getCallbackByName(cb);

    auto sprName = "square02_small.png";

    // dumb way to check if sprite exists
    if (CCSprite::create("BE_square_001_small.png"))
        sprName = "BE_square_001_small.png";

    this->m_pSprite = ButtonSprite::create(
        "", 0, 0, "goldFont.fnt", sprName, 0, .8f
    );
    this->m_pSprite->m_pBGSprite->setOpacity(100);
    this->addChild(this->m_pSprite);
    this->setCascadeColorEnabled(true);
    this->setCascadeOpacityEnabled(true);
    this->setScale(.35f);
    this->setZOrder(666);

    this->updateLabel();

    if (addToList)
        g_nodes->addObject(this);

    return true;
}

void KeybindNode::updateLabel() {
    std::string txt = "";

    if (this->m_obTarget.bind) {
        for (auto const& k : KeybindManager::get()->
            getKeybindsForCallback(
                this->m_obTarget.type,
                this->m_obTarget.bind
            )
        ) {
            txt += k.toString() + " / ";
        }
    }

    txt = txt.substr(0, txt.size() - 3);

    this->m_pSprite->setString(txt.c_str());
    this->m_pSprite->setVisible(
        txt.size() && isNodeVisible(this->m_pTarget)
    );

    this->setPosition(this->m_pTarget->convertToWorldSpace({
        this->m_pTarget->getContentSize().width / 2,
        5.0f
    }));
}

CCArray* KeybindNode::getList() {
    return g_nodes;
}

void KeybindNode::clearList() {
    g_nodes->removeAllObjects();
}

void KeybindNode::updateList() {
    CCARRAY_FOREACH_B_TYPE(g_nodes, node, KeybindNode)
        node->updateLabel();
}

void KeybindNode::showList(bool show) {
    CCARRAY_FOREACH_B_TYPE(g_nodes, node, KeybindNode)
        node->setVisible(show);
}

KeybindNode* KeybindNode::create(CCNode* t, const char* cb, bool addToList) {
    auto ret = new KeybindNode;

    if (ret && ret->init(t, cb, addToList)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
