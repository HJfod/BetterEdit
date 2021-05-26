#include <GDMake.h>
#include <GUI/CCControlExtension/CCScale9Sprite.h>

using namespace gdmake;

int strToInt(const char* str) {
    bool isStr = true;
    for (auto i = 0u; i < strlen(str); i++)
        if (!isdigit(str[i]))
            isStr = false;

    return isStr ? std::atoi(str) : -1;
}

class EUITextDelegate : public cocos2d::CCNode, public gd::TextInputDelegate {
    public:
        gd::EditorUI* m_pEditorUI;

        virtual void textChanged(gd::CCTextInputNode* input) override {
            if (input->getString() && strlen(input->getString()))
                this->m_pEditorUI->m_pEditorLayer->setCurrentLayer(strToInt(input->getString()));
            else
                this->m_pEditorUI->m_pEditorLayer->setCurrentLayer(-1);
        }

        static EUITextDelegate* create(gd::EditorUI* ui) {
            auto ret = new EUITextDelegate();

            if (ret && ret->init()) {
                ret->m_pEditorUI = ui;
                ret->autorelease();
                return ret;
            }

            CC_SAFE_DELETE(ret);
            return nullptr;
        }
};

GDMAKE_HOOK(0x907b0)
bool __fastcall EditorUI_ccTouchBegan(gd::EditorUI* self, edx_t edx, cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    auto self_ = reinterpret_cast<gd::EditorUI*>(reinterpret_cast<uintptr_t>(self) - 0xEC);
    auto i = self_->getChildByTag(6978);
    
    if (i) {
        auto inp = reinterpret_cast<gd::CCTextInputNode*>(i);
        auto isize = inp->getScaledContentSize();

        auto rect = cocos2d::CCRect {
            inp->getPositionX() - isize.width / 2,
            inp->getPositionY() - isize.height / 2,
            isize.width,
            isize.height
        };

        if (!rect.containsPoint(touch->getLocation()))
            reinterpret_cast<gd::CCTextInputNode*>(i)->getTextField()->detachWithIME();
    }

    return GDMAKE_ORIG(self, edx, touch, event);
}

GDMAKE_HOOK(0x8d7e0)
void __fastcall EditorUI_onGroupDown(gd::EditorUI* self, edx_t edx, cocos2d::CCObject* pSender) {
    GDMAKE_ORIG(self, edx, pSender);

    auto i = self->getChildByTag(6978);

    self->m_pCurrentLayerLabel->setVisible(false);

    if (i)
        reinterpret_cast<gd::CCTextInputNode*>(i)->setString(
            self->m_pCurrentLayerLabel->getString()
        );
}

GDMAKE_HOOK(0x8d780)
void __fastcall EditorUI_onGroupUp(gd::EditorUI* self, edx_t edx, cocos2d::CCObject* pSender) {
    GDMAKE_ORIG(self, edx, pSender);

    auto i = self->getChildByTag(6978);

    self->m_pCurrentLayerLabel->setVisible(false);

    if (i)
        reinterpret_cast<gd::CCTextInputNode*>(i)->setString(
            self->m_pCurrentLayerLabel->getString()
        );
}

GDMAKE_HOOK(0x88790)
void __fastcall EditorUI_onGoToBaseLayer(gd::EditorUI* self, edx_t edx, cocos2d::CCObject* pSender) {
    GDMAKE_ORIG(self, edx, pSender);

    auto i = self->getChildByTag(6978);

    self->m_pCurrentLayerLabel->setVisible(false);

    if (i)
        reinterpret_cast<gd::CCTextInputNode*>(i)->setString(
            self->m_pCurrentLayerLabel->getString()
        );
}

GDMAKE_HOOK(EditorUI::init)
bool __fastcall EditorUI_init(gd::EditorUI* self, edx_t edx, gd::GJGameLevel* lvl) {
    if (!GDMAKE_ORIG(self, edx, lvl))
        return false;

    auto ed = EUITextDelegate::create(self);

    self->m_pCurrentLayerLabel->setVisible(false);

    auto spr = cocos2d::extension::CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );

    spr->setScale(.5f);
    spr->setColor({ 0, 0, 0 });
    spr->setOpacity(100);
    spr->setContentSize({ 70.0f, 40.0f });
    spr->setPosition(self->m_pCurrentLayerLabel->getPosition());

    auto eLayerInput = gd::CCTextInputNode::create("All", ed, "bigFont.fnt", 40.0f, 30.0f);

    eLayerInput->setPosition(self->m_pCurrentLayerLabel->getPosition());
    eLayerInput->setLabelPlaceholderColor({ 120, 120, 120 });
    eLayerInput->setAllowedChars("0123456789");
    eLayerInput->setAnchorPoint({ 0, 0 });
    eLayerInput->setScale(.7f);
    eLayerInput->setDelegate(ed);
    eLayerInput->setTag(6978);
    eLayerInput->setString(self->m_pCurrentLayerLabel->getString());

    self->addChild(spr);
    self->addChild(eLayerInput);

    self->addChild(ed);

    return true;
}
