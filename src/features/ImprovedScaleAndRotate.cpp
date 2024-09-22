#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/GJScaleControl.hpp>
#include <Geode/modify/GJRotationControl.hpp>
#include <utils/Editor.hpp>
#include <numbers>

using namespace geode::prelude;

// i hate touch prioi hate touch prioi hate touch prioi hate touch prioi hate 
// touch prioi hate touch prioi hate touch prioi hate touch prioi hate touch 
// aka fix that will make you audibly say "kill yourself"
class $modify(SuperExtraEvenMoreForcePrioUI, EditorUI) {
    struct Fields {
        std::unordered_set<CCTextInputNode*> forceTouchPrio;
    };

    $override
    bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
        for (auto input : m_fields->forceTouchPrio) {
            if (input->isVisible() && CCRect(
                input->getPosition() - input->getScaledContentSize() / 2,
                input->getScaledContentSize()
            ).containsPoint(input->getParent()->convertTouchToNodeSpace(touch))) {
                return input->ccTouchBegan(touch, event);
            }
        }
        return EditorUI::ccTouchBegan(touch, event);
    }
};

// Returns radians
static float angleOfPointOnCircle(CCPoint const& point) {
    return atan2f(point.y, point.x) * (180.f / std::numbers::pi_v<float>);
}
static CCPoint pointOnCircle(float degrees, float radius) {
    return ccp(
        cosf(degrees * std::numbers::pi_v<float> / 180.f) * radius,
        sinf(degrees * std::numbers::pi_v<float> / 180.f) * radius
    );
}

static void labelLockSpr(CCNode* spr, const char* text) {
    auto labelOff = CCLabelBMFont::create(text, "bigFont.fnt");
    labelOff->setScale(.2f);
    spr->addChildAtPosition(labelOff, Anchor::Bottom);
}
static void labelLockToggle(CCMenuItemToggler* toggle, const char* text) {
    labelLockSpr(toggle->m_offButton, text);
    labelLockSpr(toggle->m_onButton, text);
}

class ScaleControlSnapLines : public CCNode {
protected:
    bool init() {
        if (!CCNode::init())
            return false;
        
        this->setID("snap-lines"_spr);
        this->setContentSize(ccp(210, 11));
        this->setAnchorPoint(ccp(.5f, .5f));

        // Smaller slider option
        // The conditionals here are horrendous but eh I was lazy
        bool big = GameManager::get()->getGameVariable("0112");
        for (size_t i = (big ? 2 : 3); i < (big ? 16 : 8); i += 1) {
            auto x = m_obContentSize.width / (big ? 15 : 6) * (i - (big ? 1 : 2));
            auto spr = CCSprite::createWithSpriteFrameName(
                (i % 4) ? "slider-tick-small.png"_spr : "slider-tick.png"_spr
            );
            spr->setOpacity((i % 4) ? 105 : 255);
            this->addChildAtPosition(spr, Anchor::Left, ccp(x, 0));
        }

        return true;
    }

public:
    static ScaleControlSnapLines* create() {
        auto ret = new ScaleControlSnapLines();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

class $modify(GJScaleControl) {
    $override
    bool init() {
        if (!GJScaleControl::init())
            return false;
        
        m_scaleXLabel->setPositionX(-30);
        m_scaleYLabel->setPositionX(-30);
        m_scaleLabel->setPositionX(-25);

        auto inputX = TextInput::create(50, "Num");
        inputX->setScale(.8f);
        inputX->setID("input-x"_spr);
        inputX->setPosition(30, m_scaleXLabel->getPositionY());
        inputX->setCommonFilter(CommonFilter::Float);
        inputX->setCallback([this, inputX](auto const& str) {
            if (auto scale = numFromString<float>(str)) {
                m_delegate->scaleXChanged(*scale, m_scaleLocked);
                m_sliderX->setValue(this->valueFromScale(*scale));
            }
        });
        this->addChild(inputX);

        auto inputY = TextInput::create(50, "Num");
        inputY->setScale(.8f);
        inputY->setID("input-y"_spr);
        inputY->setPosition(30, m_scaleYLabel->getPositionY());
        inputY->setCommonFilter(CommonFilter::Float);
        inputY->setCallback([this, inputY](auto const& str) {
            if (auto scale = numFromString<float>(str)) {
                m_delegate->scaleYChanged(*scale, m_scaleLocked);
                m_sliderY->setValue(this->valueFromScale(*scale));
            }
        });
        this->addChild(inputY);

        auto inputXY = TextInput::create(50, "Num");
        inputXY->setScale(.8f);
        inputXY->setID("input-xy"_spr);
        inputXY->setPosition(25, m_scaleLabel->getPositionY());
        inputXY->setCommonFilter(CommonFilter::Float);
        inputXY->setCallback([this, inputXY](auto const& str) {
            if (auto scale = numFromString<float>(str)) {
                m_delegate->scaleXYChanged(*scale, *scale, m_scaleLocked);
                m_sliderXY->setValue(this->valueFromScale(*scale));
            }
        });
        this->addChild(inputXY);

        auto menu = m_scaleLockButton->getParent();
        menu->setID("lock-menu"_spr);
        menu->setContentWidth(50);

        labelLockSpr(m_scaleLockButton->getNormalImage(), "Pos");

        auto snapBtn = CCMenuItemToggler::create(
            CCSprite::createWithSpriteFrameName("warpLockOffBtn_001.png"),
            CCSprite::createWithSpriteFrameName("warpLockOnBtn_001.png"),
            this, nullptr
        );
        labelLockToggle(snapBtn, "Snap");
        snapBtn->setID("snap-lock"_spr);
        menu->addChild(snapBtn);

        menu->setLayout(RowLayout::create());

        m_sliderX->addChild(ScaleControlSnapLines::create());
        m_sliderX->m_touchLogic->setZOrder(1);
        m_sliderY->addChild(ScaleControlSnapLines::create());
        m_sliderY->m_touchLogic->setZOrder(1);
        m_sliderXY->addChild(ScaleControlSnapLines::create());
        m_sliderXY->m_touchLogic->setZOrder(1);

        return true;
    }

    $override
    void loadValues(GameObject* obj, CCArray* objs, gd::unordered_map<int, GameObjectEditorState>& states) {
        GJScaleControl::loadValues(obj, objs, states);

        this->updateInput(this->getInputX());
        this->updateInput(this->getInputY());
        this->updateInput(this->getInputXY());

        auto ui = static_cast<SuperExtraEvenMoreForcePrioUI*>(m_delegate);
        ui->m_fields->forceTouchPrio.insert(this->getInputX()->getInputNode());
        ui->m_fields->forceTouchPrio.insert(this->getInputY()->getInputNode());
        ui->m_fields->forceTouchPrio.insert(this->getInputXY()->getInputNode());
    }
    $override
    void updateLabelX(float scale) {
        GJScaleControl::updateLabelX(scale);
        this->updateInput(this->getInputX());
    }
    $override
    void updateLabelY(float scale) {
        GJScaleControl::updateLabelY(scale);
        this->updateInput(this->getInputY());
    }
    $override
    void updateLabelXY(float scale) {
        GJScaleControl::updateLabelXY(scale);
        this->updateInput(this->getInputXY());
    }

    // Why is m_scaleLockButton not a CCMenuItemToggler??
    $override
    void onToggleLockScale(CCObject* sender) {
        GJScaleControl::onToggleLockScale(sender);
        labelLockSpr(m_scaleLockButton->getNormalImage(), "Pos");
    }

    $override
    void ccTouchMoved(CCTouch* touch, CCEvent* event) {
        // Call original without the delegate so it doesn't fire an unnecessary 
        // angleChanged event
        auto delegate = m_delegate;
        m_delegate = nullptr;
        GJScaleControl::ccTouchMoved(touch, event);
        m_delegate = delegate;

        this->getInputX()->defocus();
        this->getInputY()->defocus();
        this->getInputXY()->defocus();

        auto scaleX = this->scaleFromValue(m_sliderX->getThumb()->getValue());
        auto scaleY = this->scaleFromValue(m_sliderY->getThumb()->getValue());
        auto ratio = scaleY / scaleX;
        if (auto lock = this->getSnapLock(); lock && lock->isToggled()) {
            scaleX = roundf(scaleX / .25f) * .25f;
            scaleY = roundf(scaleY / .25f) * .25f;
        }

        if (m_scaleButtonType == 0) {
            m_delegate->scaleXChanged(scaleX, m_scaleLocked);
            m_sliderX->setValue(this->valueFromScale(scaleX));
            this->getInputX()->setString(numToString(scaleX, 3));
        }
        else if (m_scaleButtonType == 1) {
            m_delegate->scaleYChanged(scaleY, m_scaleLocked);
            m_sliderY->setValue(this->valueFromScale(scaleY));
            this->getInputY()->setString(numToString(scaleY, 3));
        }
        else {
            float scale = scaleX;
            if (scaleX < scaleY) {
                scale = scaleY;
                m_delegate->scaleXYChanged(scaleY / ratio, scaleY, m_scaleLocked);
            }
            else {
                m_delegate->scaleXYChanged(scaleX, scaleX * ratio, m_scaleLocked);
            }
            m_sliderXY->setValue(this->valueFromScale(scale));
            this->getInputXY()->setString(numToString(scale, 3));
        }
    }

    void updateInput(TextInput* input) {
        CCLabelBMFont* label;
        Slider* slider;
        const char* text;
        if (input->getID() == "input-x"_spr) {
            label = m_scaleXLabel;
            slider = m_sliderX;
            text = "Scale X:";
        }
        else if (input->getID() == "input-y"_spr) {
            label = m_scaleYLabel;
            slider = m_sliderY;
            text = "Scale Y:";
        }
        else {
            label = m_scaleLabel;
            slider = m_sliderXY;
            text = "Scale:";
        }
        input->setVisible(label->isVisible());
        if (label->isVisible()) {
            label->setString(text);
            label->setScale(.5f);
            input->setString(numToString(this->scaleFromValue(slider->getThumb()->getValue())));
        }
    }

    TextInput* getInputX() {
        return static_cast<TextInput*>(this->getChildByID("input-x"_spr));
    }
    TextInput* getInputY() {
        return static_cast<TextInput*>(this->getChildByID("input-y"_spr));
    }
    TextInput* getInputXY() {
        return static_cast<TextInput*>(this->getChildByID("input-xy"_spr));
    }
    CCMenuItemToggler* getSnapLock() {
        return static_cast<CCMenuItemToggler*>(this->querySelector(
            "hjfod.betteredit/lock-menu hjfod.betteredit/snap-lock"
        ));
    }
};

class $modify(InputRotationControl, GJRotationControl) {
    $override
    bool init() {
        if (!GJRotationControl::init())
            return false;
        
        // todo: you can place blocks through these
        auto input = TextInput::create(50, "Num");
        input->setScale(.8f);
        input->setID("input-angle"_spr);
        input->setPosition(110, 0);
        input->setCommonFilter(CommonFilter::Float);
        input->setCallback([this, input](auto const& str) {
            if (auto angle = numFromString<float>(str)) {
                m_delegate->angleChangeBegin();
                m_delegate->angleChanged(*angle);
                m_delegate->angleChangeEnded();
                this->setThumbValue(*angle);
            }
        });
        this->addChild(input);

        auto menu = CCMenu::create();
        menu->setContentWidth(50);
        menu->setID("lock-menu"_spr);

        auto posBtn = CCMenuItemToggler::create(
            CCSprite::createWithSpriteFrameName("warpLockOffBtn_001.png"),
            CCSprite::createWithSpriteFrameName("warpLockOnBtn_001.png"),
            this, nullptr
        );
        labelLockToggle(posBtn, "Pos");
        posBtn->setID("pos-lock"_spr);
        menu->addChild(posBtn);

        auto snapBtn = CCMenuItemToggler::create(
            CCSprite::createWithSpriteFrameName("warpLockOffBtn_001.png"),
            CCSprite::createWithSpriteFrameName("warpLockOnBtn_001.png"),
            this, nullptr
        );
        labelLockToggle(snapBtn, "Snap");
        snapBtn->setID("snap-lock"_spr);
        menu->addChild(snapBtn);

        menu->setLayout(RowLayout::create());
        menu->setPosition(110, 35);
        this->addChild(menu);

        return true;
    }

    $override
    void draw() {
        GJRotationControl::draw();
        // Draw large ticks every 45° and small ticks every 15°
        for (size_t i = 0; i < 24; i += 1) {
            glLineWidth((i % 3) ? 1 : 2);
            float len = (i % 3) ? 2.5f : 5;
            float angle = i * 15;
            ccDrawLine(pointOnCircle(angle, 60 - len), pointOnCircle(angle, 60 + len));
        }
    }

    $override
    void ccTouchMoved(CCTouch* touch, CCEvent* event) {
        // Call original without the delegate so it doesn't fire an unnecessary 
        // angleChanged event
        auto delegate = m_delegate;
        m_delegate = nullptr;
        GJRotationControl::ccTouchMoved(touch, event);
        m_delegate = delegate;

        auto input = this->getInput();
        input->defocus();

        auto angle = this->getThumbValue();
        if (auto lock = this->getSnapLock(); lock && lock->isToggled()) {
            angle = roundf(this->getThumbValue() / 15) * 15;
            m_controlSprite->setPosition(pointOnCircle(-angle + 90, 60));
        }
        m_delegate->angleChanged(angle);
        input->setString(numToString(angle, 3));
    }

    void myLoadValues(std::vector<GameObject*> const& objs) {
        if (objs.empty()) return;
        auto angle = objs.front()->getRotation();
        this->setThumbValue(angle);
        this->getInput()->setString(numToString(angle, 3));

        auto ui = static_cast<SuperExtraEvenMoreForcePrioUI*>(m_delegate);
        ui->m_fields->forceTouchPrio.insert(this->getInput()->getInputNode());
    }

    float getThumbValue() const {
        return -angleOfPointOnCircle(m_controlPosition) + 90;
    }
    void setThumbValue(float value) {
        value = -value + 90;
        m_controlPosition = pointOnCircle(value, 60);
        m_controlSprite->setPosition(m_controlPosition);
    }

    TextInput* getInput() {
        return static_cast<TextInput*>(this->getChildByID("input-angle"_spr));
    }
    CCMenuItemToggler* getSnapLock() {
        return static_cast<CCMenuItemToggler*>(this->querySelector(
            "hjfod.betteredit/lock-menu hjfod.betteredit/snap-lock"
        ));
    }
    CCMenuItemToggler* getPosLock() {
        return static_cast<CCMenuItemToggler*>(this->querySelector(
            "hjfod.betteredit/lock-menu hjfod.betteredit/pos-lock"
        ));
    }
};

class $modify(EditorUI) {
    struct Fields {
        bool lockRotationPosition = false;
    };
    
    $override
    void activateRotationControl(CCObject* sender) {
        EditorUI::activateRotationControl(sender);
        static_cast<InputRotationControl*>(m_rotationControl)->myLoadValues(::getSelectedObjects(this));
    }
    // Make angleChanged be absolute rotation instead of relative
    $override
    void angleChanged(float angle) {
        if (0) { return EditorUI::angleChanged(angle); }
        CCArray* objs = nullptr;
        bool lockPos = static_cast<InputRotationControl*>(m_rotationControl)->getPosLock()->isToggled();
        if (m_selectedObjects && m_selectedObjects->count()) {
            objs = m_selectedObjects;
        }
        else if (m_selectedObject) {
            objs = CCArray::createWithObject(m_selectedObject);
            m_pivotPoint = m_selectedObject->getPosition();
        }
        if (objs) {
            auto orig = static_cast<GameObject*>(objs->firstObject())->getRotation();
            m_fields->lockRotationPosition = lockPos;
            this->rotateObjects(objs, -orig + angle, m_pivotPoint);
            m_fields->lockRotationPosition = false;
        }
    }
    
    $override
    void moveObject(GameObject* obj, CCPoint amount) {
        if (!m_fields->lockRotationPosition) {
            EditorUI::moveObject(obj, amount);
        }
    }
};
