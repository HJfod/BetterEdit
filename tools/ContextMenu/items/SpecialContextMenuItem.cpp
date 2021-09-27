#include "../ContextMenu.hpp"
#include "../CustomizeCMLayer.hpp"
#include "../../EditorLayerInput/editorLayerInput.hpp"

#define MORD(x, d) (this->m_pMenu ? this->m_pMenu->x : d)

bool SpecialContextMenuItem::init(
    ContextMenu* menu,
    const char* spr,
    const char* txt,
    SpecialContextMenuItem::Callback cb
) {
    if (!ContextMenuItem::init(menu))
        return false;
    
    if (spr) {
        this->m_pSprite = CCSprite::createWithSpriteFrameName(spr);
        if (this->m_pSprite)
            this->addChild(this->m_pSprite);
    }

    this->m_pLabel = this->createLabel(txt);
    this->m_pLabel->setColor(to3B(m_pMenu->m_colText));
    this->addChild(this->m_pLabel);

    this->m_pCallback = cb;

    return true;
}

void SpecialContextMenuItem::visit() {
    auto hasLabel = this->m_pLabel->getString() && strlen(this->m_pLabel->getString());
    if (hasLabel) {
        this->m_pLabel->limitLabelWidth(
            this->getContentSize().width - 30.f,
            this->m_pMenu ? this->m_pMenu->m_fFontScale : .4f,
            .02f
        );
        this->m_pLabel->limitLabelHeight(
            this->getContentSize().height - 3.f,
            this->m_pMenu ? this->m_pMenu->m_fFontScale : .4f,
            .02f
        );
    }

    if (this->m_pSprite) {
        limitNodeSize(this->m_pSprite,
            this->getContentSize() - 4.f, this->m_fSpriteScale, .02f
        );
    }

    if (hasLabel) {
        this->m_pLabel->setPosition({
            this->getContentSize().width / 2 + (this->m_pSprite ? (
                1.0f + this->m_pSprite->getScaledContentSize().width / 2
            ) : 0.f),
            this->getContentSize().height / 2
        });
    }

    if (this->m_pSprite) {
        this->m_pSprite->setPosition({
            this->getContentSize().width / 2 + (hasLabel ? (
                - 1.0f - this->m_pLabel->getScaledContentSize().width / 2 
            ) : 0.f),
            this->getContentSize().height / 2
        });
    }

    ContextMenuItem::visit();
}

void SpecialContextMenuItem::activate() {
    if (this->m_pCallback) {
        if (!this->m_pCallback(this)) {
            SuperMouseManager::get()->releaseCapture(this);
            if (this->m_pMenu)
                this->m_pMenu->hide();
        }
    }
}

void SpecialContextMenuItem::setSpriteOpacity(GLubyte b) {
    this->m_nSpriteOpacity = b;
    if (this->m_pSprite)
        this->m_pSprite->setOpacity(b);
}

SpecialContextMenuItem* SpecialContextMenuItem::create(
    ContextMenu* menu,
    const char* spr,
    const char* txt,
    SpecialContextMenuItem::Callback cb
) {
    auto ret = new SpecialContextMenuItem;

    if (ret && ret->init(menu, spr, txt, cb)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
