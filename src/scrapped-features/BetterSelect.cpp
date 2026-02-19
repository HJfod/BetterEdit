#ifdef BETTEREDIT_INCLUDE_SCRAPPED_WARP

#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/GJTransformControl.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <utils/Editor.hpp>

using namespace geode::prelude;

class SelectionDraw : public CCNode {
protected:
    ccColor3B m_color = ccWHITE;

    bool init() override {
        if (!CCNode::init())
            return false;
        
        this->setID("selection-rect"_spr);
        this->setAnchorPoint(CCPointZero);

        return true;
    }

    void draw() override {
        ccDrawColor4B(to4B(m_color));
        ccDrawRect(CCPointZero, m_obContentSize);

        const auto scale = LevelEditorLayer::get()->m_objectLayer->getScale();
        const auto drawHandle = [this, scale](CCPoint const& pos) {
            ccDrawSolidRect(pos - ccp(2, 2) / scale, pos + ccp(2, 2) / scale, to4F(to4B(m_color)));
            ccDrawSolidRect(pos - ccp(1.5f, 1.5f) / scale, pos + ccp(1.5f, 1.5f) / scale, to4F(to4B(ccWHITE)));
        };
        drawHandle(ccp(0, 0));
        drawHandle(ccp(0, m_obContentSize.height / 2));
        drawHandle(ccp(0, m_obContentSize.height));
        drawHandle(ccp(m_obContentSize.width / 2, m_obContentSize.height));
        drawHandle(ccp(m_obContentSize.width, m_obContentSize.height));
        drawHandle(ccp(m_obContentSize.width, m_obContentSize.height / 2));
        drawHandle(ccp(m_obContentSize.width, 0));
        drawHandle(ccp(m_obContentSize.width / 2, 0));
    }

public:
    static SelectionDraw* create() {
        auto ret = new SelectionDraw();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
    void setColor(ccColor3B color) {
        m_color = color;
    }
};

class $modify(EpicTransformControl, GJTransformControl) {
    static constexpr int TAG_LEFT         = 2;
    static constexpr int TAG_RIGHT        = 3;
    static constexpr int TAG_TOP          = 4;
    static constexpr int TAG_BOTTOM       = 5;
    static constexpr int TAG_TOP_LEFT     = 6;
    static constexpr int TAG_TOP_RIGHT    = 7;
    static constexpr int TAG_BOTTOM_LEFT  = 8;
    static constexpr int TAG_BOTTOM_RIGHT = 9;

    static bool isHandleTag(int tag) {
        return tag >= TAG_LEFT && tag <= TAG_BOTTOM_RIGHT;
    }
    static int opposite(int tag) {
        switch (tag) {
            default:
            case TAG_LEFT: return TAG_RIGHT;
            case TAG_RIGHT: return TAG_LEFT;
            case TAG_TOP: return TAG_BOTTOM;
            case TAG_BOTTOM: return TAG_TOP;
            case TAG_TOP_LEFT: return TAG_BOTTOM_RIGHT;
            case TAG_BOTTOM_RIGHT: return TAG_TOP_LEFT;
            case TAG_TOP_RIGHT: return TAG_BOTTOM_LEFT;
            case TAG_BOTTOM_LEFT: return TAG_TOP_RIGHT;
        }
    }

    struct Fields {
        SelectionDraw* draw;
        CCMenuItemToggler* gridLock;
    };

    $override
    bool init() {
        if (!GJTransformControl::init())
            return false;
        
        if (!Mod::get()->template getSettingValue<bool>("better-warp-tools"))
            return true;
        
        m_fields->draw = SelectionDraw::create();
        this->addChild(m_fields->draw);

        auto menu = m_warpLockButton->getParent();
        auto gridSnapOnSpr  = CircleButtonSprite::createWithSpriteFrameName("v-grid.png"_spr, 1.f, CircleBaseColor::Cyan);
        gridSnapOnSpr->setScale(.35f);
        auto gridSnapOffSpr = CircleButtonSprite::createWithSpriteFrameName("v-grid.png"_spr, 1.f, CircleBaseColor::Gray);
        gridSnapOffSpr->setScale(.35f);
        m_fields->gridLock = CCMenuItemToggler::create(
            gridSnapOffSpr, gridSnapOnSpr,
            nullptr, nullptr
        );
        m_fields->gridLock->setPosition(0, -20);
        menu->addChild(m_fields->gridLock);

        return true;
    }

    $override
    bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
        if (!GJTransformControl::ccTouchBegan(touch, event))
            return false;
        
        if (m_fields->draw) {
            if (isHandleTag(m_transformButtonType)) {
                this->spriteByTag(1)->setPosition(
                    this->spriteByTag(opposite(m_transformButtonType))->getPosition()
                );
                m_transformButtonType = 1;
                GJTransformControl::ccTouchEnded(touch, event);
                GJTransformControl::ccTouchBegan(touch, event);
            }
            if (m_fields->gridLock->isToggled()) {
                m_cursorDifference = CCPointZero;
            }
        }
        
        return true;
    }
    $override
    void ccTouchMoved(CCTouch* touch, CCEvent* event) {
        if (m_fields->gridLock && m_fields->gridLock->isToggled()) {
            m_cursorDifference = CCPointZero;
            auto lel = LevelEditorLayer::get();
            auto grid = ccp(lel->m_editorUI->m_gridSize, lel->m_editorUI->m_gridSize);
            touch->m_point = lel->m_objectLayer->convertToWorldSpace(
                lel->m_editorUI->getGridSnappedPos(
                    lel->m_objectLayer->convertToNodeSpace(touch->m_point) + grid / 2
                ) - grid / 2
            );
        }
        GJTransformControl::ccTouchMoved(touch, event);
        // log::info("m_unk2: {}", m_unk2);
    }
    $override
    void ccTouchEnded(CCTouch* touch, CCEvent* event) {
        if (m_fields->draw && m_transformButtonType != 1) {
            this->spriteByTag(1)->setPosition(m_fields->draw->getPosition() + m_fields->draw->getContentSize() / 2);
        }
        GJTransformControl::ccTouchEnded(touch, event);
    }

    $override
    void updateButtons(bool idk0, bool idk1) {
        GJTransformControl::updateButtons(idk0, idk1);

        if (auto draw = m_fields->draw) {
            for (auto sprite : CCArrayExt<CCNode*>(m_warpSprites)) {
                if (isHandleTag(sprite->getTag())) {
                    sprite->setVisible(false);
                }
            }
            draw->setPosition(
                this->spriteByTag(TAG_BOTTOM_LEFT)->getPosition()
            );
            draw->setContentSize(
                this->spriteByTag(TAG_TOP_RIGHT)->getPosition() - 
                    this->spriteByTag(TAG_BOTTOM_LEFT)->getPosition()
            );
            draw->setRotation(m_mainNode->getRotation());
        }
    }
};

class $modify(GameObject) {
    $override
    void selectObject(ccColor3B color) {
        if (auto select = static_cast<SelectionDraw*>(
            EditorUI::get()->m_transformControl->querySelector("selection-rect"_spr)
        )) {
            select->setColor(color);
        }
        else {
            GameObject::selectObject(color);
        }
    }
};

class $modify(EditorUI) {
    $override
    void selectObject(GameObject* obj, bool undo) {
        EditorUI::selectObject(obj, undo);
        if (Mod::get()->template getSettingValue<bool>("better-warp-tools")) {
            this->activateTransformControl(nullptr);
        }
    }
    $override
    void selectObjects(CCArray* objs, bool undo) {
        EditorUI::selectObjects(objs, undo);
        if (Mod::get()->template getSettingValue<bool>("better-warp-tools")) {
            this->activateTransformControl(nullptr);
        }
    }
};

#endif
