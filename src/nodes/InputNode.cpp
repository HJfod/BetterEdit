#include "InputNode.hpp"

const char* InputNode::getString() {
    return this->m_pInput->getTextField()->getString();
}

void InputNode::setString(const char* _str) {
    this->m_pInput->getTextField()->setString(_str);

    this->m_pInput->refreshLabel();
}

bool InputNode::init(float _w, float _h, const char* _phtxt, const char* _fnt, const std::string & _awc, int _cc) {
    this->m_pBG = cocos2d::extension::CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );

    this->m_pBG->setScale(.5f);
    this->m_pBG->setColor({ 0, 0, 0 });
    this->m_pBG->setOpacity(75);
    this->m_pBG->setContentSize({ _w * 2, _h * 2 });

    this->addChild(this->m_pBG);

    this->m_pInput = gd::CCTextInputNode::create(
        _phtxt, this, _fnt, _w - 10.0f, 60.0f
    );

    this->m_pInput->setLabelPlaceholderColor({ 150, 150, 150 });
    this->m_pInput->setLabelPlaceholerScale(.75f);
    this->m_pInput->setMaxLabelScale(.8f);
    this->m_pInput->setMaxLabelLength(_cc);
    if (_awc.length())
        this->m_pInput->setAllowedChars(_awc);

    this->addChild(this->m_pInput);

    return true;
}

bool InputNode::init(float _w, const char* _phtxt, const char* _fnt, const std::string & _awc, int _cc) {
    return init(_w, 30.0f, _phtxt, _fnt, _awc, _cc);
}

bool InputNode::initMulti(float _w, float _h, const char* _phtxt, const char* _fnt, const std::string & _awc, int _cc) {
    auto ret = init(_w, _h, _phtxt, _fnt, _awc, _cc);

    //this->m_pInput->setLineLength(_w);

    return ret;
}

InputNode* InputNode::create(float _w, const char* _phtxt, const char* _fnt, const std::string & _awc, int _cc) {
    auto pRet = new InputNode();

    if (pRet && pRet->init(_w, _phtxt, _fnt, _awc, _cc)) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}

InputNode* InputNode::create(float _w, const char* _phtxt, const std::string & _awc) {
    return create(_w, _phtxt, "bigFont.fnt", _awc, 69);
}

InputNode* InputNode::create(float _w, const char* _phtxt, const std::string & _awc, int _cc) {
    return create(_w, _phtxt, "bigFont.fnt", _awc, _cc);
}

InputNode* InputNode::create(float _w, const char* _phtxt, const char* _fnt) {
    return create(_w, _phtxt, _fnt, "", 69);
}

InputNode* InputNode::create(float _w, const char* _phtxt) {
    return create(_w, _phtxt, "bigFont.fnt");
}

InputNode* InputNode::createMulti(float _w, float _h, const char* _phtxt, const char* _fnt, int _cc) {
    auto pRet = new InputNode();

    if (pRet && pRet->initMulti(_w, _h, _phtxt, _fnt, "", _cc)) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}

InputNode* InputNode::createMulti(float _w, float _h, const char* _phtxt, const char* _fnt, const std::string & _awc, int _cc) {
    auto pRet = new InputNode();

    if (pRet && pRet->initMulti(_w, _h, _phtxt, _fnt, _awc, _cc)) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}

