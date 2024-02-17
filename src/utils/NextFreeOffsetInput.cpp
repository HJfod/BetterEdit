#include "NextFreeOffsetInput.hpp"

// These aren't on Windows wtf...

#ifdef GEODE_IS_WINDOWS
void cocos2d::ccArrayDoubleCapacity(ccArray *arr)
{
	arr->max *= 2;
	CCObject** newArr = (CCObject**)realloc( arr->arr, arr->max * sizeof(CCObject*) );
	// will fail when there's not enough memory
    CCAssert(newArr != 0, "ccArrayDoubleCapacity failed. Not enough memory");
	arr->arr = newArr;
}

void cocos2d::ccArrayEnsureExtraCapacity(ccArray *arr, unsigned int extra)
{
	while (arr->max < arr->num + extra)
    {
		ccArrayDoubleCapacity(arr);
    }
}
#endif

bool NextFreeOffsetInput::init() {
    if (!CCNode::init())
        return false;
    
    this->setAnchorPoint({ .5f, .5f });
    this->setContentSize({ 40, 30 });

    auto label = CCLabelBMFont::create("Offset", "goldFont.fnt");
    label->setScale(.35f);
    this->addChildAtPosition(label, Anchor::Top, ccp(0, -5));
    
    m_input = TextInput::create(60.f, "Off");
    m_input->setCommonFilter(CommonFilter::Uint);
    m_input->setScale(.5f);
    this->addChildAtPosition(m_input, Anchor::Bottom, ccp(0, 10));

    return true;
}

NextFreeOffsetInput* NextFreeOffsetInput::create() {
    auto ret = new NextFreeOffsetInput();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
