#include "KeybindNode.hpp"

CCArray* g_nodes = nullptr;

bool KeybindNode::init(const char* cb, bool addToList) {
    if (!this->CCSprite::init())
        return false;

    if (!g_nodes) {
        g_nodes = CCArray::create();
        g_nodes->retain();
    }

    this->m_obTarget = KeybindManager::get()->getCallbackByName(cb);

    auto sprName = "square02_small.png";

    // dumb way to check if sprite exists
    if (CCSprite::create("BE_square_001_small.png"))
        sprName = "BE_square_001_small.png";

    this->m_pSprite = ButtonSprite::create(
        "", 0, 0, "goldFont.fnt", sprName, 0, .8f
    );
    this->m_pSprite->m_pBGSprite->setOpacity(50);
    this->addChild(this->m_pSprite);
    this->setCascadeColorEnabled(true);
    this->setCascadeOpacityEnabled(true);
    this->setScale(.35f);

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
}

CCArray* KeybindNode::getList() {
    return g_nodes;
}

void KeybindNode::clearList() {
    g_nodes->removeAllObjects();
}

void KeybindNode::showList(bool show) {
    CCARRAY_FOREACH_B_TYPE(g_nodes, node, KeybindNode)
        node->setVisible(show);
}

KeybindNode* KeybindNode::create(const char* cb, bool addToList) {
    auto ret = new KeybindNode;

    if (ret && ret->init(cb, addToList)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
