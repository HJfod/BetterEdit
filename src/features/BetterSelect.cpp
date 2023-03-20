#include <MoreTabs.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <other/Utils.hpp>
#include <tracking/Tracking.hpp>

using namespace std::chrono;
using Clock = high_resolution_clock;

static CCMenuItemToggler* createEditorButton(
    const char* top,
    CCObject* target,
    SEL_MenuHandler selector
) {
    auto off = ButtonSprite::create(
        CCSprite::createWithSpriteFrameName(top),
        0x32, true, 0x32, "GJ_button_01.png", .9f
    );
    off->setScale(.65f);
    auto on = ButtonSprite::create(
        CCSprite::createWithSpriteFrameName(top),
        0x32, true, 0x32, "GJ_button_02.png", .9f
    );
    on->setScale(.65f);
    return CCMenuItemToggler::create(off, on, target, selector);
}

enum class SelectTool {
    Swipe,
    Lasso,
    MagicWand,
};

static const char* getToolSprite(SelectTool tool) {
    switch (tool) {
        default:
        case SelectTool::Swipe: return "edit_swipeBtn_001.png";
        case SelectTool::Lasso: return "lasso.png"_spr;
        case SelectTool::MagicWand: return "magic-wand.png"_spr;
    }
}

enum class SelectMode {
    Unique,
    Add,
    Subtract,
    Intersect,
    XOR,
};

static const char* getModeSprite(SelectMode type) {
    switch (type) {
        default:
        case SelectMode::Unique: return "select-unique.png"_spr;
        case SelectMode::Add: return "select-add.png"_spr;
        case SelectMode::Subtract: return "select-subtract.png"_spr;
        case SelectMode::Intersect: return "select-intersect.png"_spr;
        case SelectMode::XOR: return "select-xor.png"_spr;
    }
}

class ASelect : public CCNode {
protected:
    EditorUI* m_ui;
    CCMenu* m_selectToolMenu;
    CCMenu* m_selectTypeMenu;
    std::vector<CCMenuItemToggler*> m_tools;
    std::vector<CCMenuItemToggler*> m_types;
    static inline SelectTool s_tool = SelectTool::Swipe;
    static inline SelectMode s_mode = SelectMode::Add;

    bool init(EditorUI* ui) {
        if (!CCNode::init())
            return false;
        
        m_ui = ui;
        
        auto winSize = CCDirector::get()->getWinSize();

        m_selectToolMenu = CCMenu::create();
        m_selectToolMenu->ignoreAnchorPointForPosition(false);

        this->addTool(SelectTool::Swipe);
        this->addTool(SelectTool::Lasso);
        this->addTool(SelectTool::MagicWand);
        
        this->addChild(m_selectToolMenu);

        m_selectTypeMenu = CCMenu::create();
        m_selectTypeMenu->ignoreAnchorPointForPosition(false);

        this->addType(SelectMode::Unique);
        this->addType(SelectMode::Add);
        this->addType(SelectMode::Subtract);
        this->addType(SelectMode::Intersect);
        this->addType(SelectMode::XOR);
        
        this->addChild(m_selectTypeMenu);

        this->updateTool();
        this->updateMode();

        return true;
    }

    CCMenuItemToggler* addTool(SelectTool to) {
        auto tool = createEditorButton(
            getToolSprite(to), this, menu_selector(ASelect::onSelectTool)
        );
        tool->setTag(static_cast<int>(to));
        m_tools.push_back(tool);
        m_selectToolMenu->addChild(tool);
        return tool;
    }

    CCMenuItemToggler* addType(SelectMode ty) {
        auto type = createEditorButton(
            getModeSprite(ty), this, menu_selector(ASelect::onSelectMode)
        );
        type->setTag(static_cast<int>(ty));
        m_types.push_back(type);
        m_selectTypeMenu->addChild(type);
        return type;
    }

    void onSelectTool(CCObject* sender) {
        for (auto& tool : m_tools) {
            if (tool != sender) {
                tool->toggle(false);
            }
            else {
                s_tool = static_cast<SelectTool>(tool->getTag());
                // this code makes no sense until you remember that 
                // CCMenuItemToggler toggles after the callback
                m_ui->m_swipeEnabled = tool->isToggled();
                m_ui->toggleSwipe(nullptr);
                tool->toggle(!m_ui->m_swipeEnabled);
            }
        }
    }

    void onSelectMode(CCObject* sender) {
        for (auto& type : m_types) {
            type->toggle(false);
            if (type == sender) {
                s_mode = static_cast<SelectMode>(type->getTag());
            }
        }
        this->updateEditorUI();
    }

    virtual void updateEditorUI();

public:
    void updateTool() {
        for (auto& tool : m_tools) {
            tool->toggle(
                m_ui->m_swipeEnabled &&
                static_cast<SelectTool>(tool->getTag()) == s_tool
            );
        }
    }

    void updateMode() {
        for (auto& type : m_types) {
            type->toggle(
                static_cast<SelectMode>(type->getTag()) == s_mode
            );
        }
    }

    static SelectTool getTool() {
        return s_tool;
    }

    static SelectMode getMode() {
        return s_mode;
    }
};

class BetterSelect : public ASelect {
protected:
    bool init(EditorUI* ui) {
        if (!ASelect::init(ui))
            return false;
        
        auto winSize = CCDirector::get()->getWinSize();

        m_selectToolMenu->setContentSize({ 110.f, 50.f });
        m_selectToolMenu->setPosition(winSize.width / 2 - 70.f, 45.f);
        m_selectToolMenu->setLayout(
            RowLayout::create()
                ->setGrowCrossAxis(true)
                ->setAxisAlignment(AxisAlignment::End)
        );

        m_selectTypeMenu->setContentSize({ 110.f, 50.f });
        m_selectTypeMenu->setPosition(winSize.width / 2 + 70.f, 45.f);
        m_selectTypeMenu->setLayout(
            RowLayout::create()
                ->setGrowCrossAxis(true)
                ->setAxisAlignment(AxisAlignment::Start)
        );

        return true;
    }

    void updateEditorUI() override;

public:
    static BetterSelect* create(EditorUI* ui) {
        auto ret = new BetterSelect;
        if (ret && ret->init(ui)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

class QuickSelect : public ASelect {
protected:
    bool init(EditorUI* ui) {
        if (!ASelect::init(ui))
            return false;

        auto bg = CCScale9Sprite::createWithSpriteFrameName("border-square.png"_spr);
        bg->setContentSize({ 80.f, 170.f });
        bg->setZOrder(-1);
        this->addChild(bg);

        auto winSize = CCDirector::get()->getWinSize();

        m_selectToolMenu->setContentSize({ 40.f, 150.f });
        m_selectToolMenu->setPosition(-20.f, 0.f);
        m_selectToolMenu->setLayout(
            ColumnLayout::create()
                ->setAxisAlignment(AxisAlignment::Start)
        );

        m_selectTypeMenu->setContentSize({ 40.f, 150.f });
        m_selectTypeMenu->setPosition(20.f, 0.f);
        m_selectTypeMenu->setLayout(
            ColumnLayout::create()
                ->setAxisAlignment(AxisAlignment::Start)
        );

        this->setZOrder(101);
        this->setScale(.65f);

        return true;
    }

    void updateEditorUI() override;

public:
    static QuickSelect* create(EditorUI* ui) {
        auto ret = new QuickSelect();
        if (ret && ret->init(ui)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

class $modify(SelectUI, EditorUI) {
    BetterSelect* select = nullptr;
    std::vector<CCPoint> lassoPoints;
    Ref<CCArray> selection;
    std::vector<Ref<GameObject>> deselection;
    Clock::time_point lastSwipeClick;
    QuickSelect* quickSelect = nullptr;

    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;

        MoreTabs::get(this)->addEditTab(
            "edit_swipeBtn_001.png",
            m_fields->select = BetterSelect::create(this)
        );
        this->updateSwipeImage();

        return true;
    }

    CCArray* filterSwipe(CCArray* src) {
        switch (m_fields->select->getMode()) {
            case SelectMode::Unique: {
                return src;
            } break;

            case SelectMode::Add: {
                src->addObjectsFromArray(this->getSelectedObjects());
                return src;
            } break;

            case SelectMode::Subtract: {
                auto res = CCArray::create();
                for (auto obj : iterSelected(this)) {
                    if (!src->containsObject(obj)) {
                        res->addObject(obj);
                    }
                }
                return res;
            } break;

            case SelectMode::Intersect: {
                auto res = CCArray::create();
                for (auto obj : iterSelected(this)) {
                    if (src->containsObject(obj)) {
                        res->addObject(obj);
                    }
                }
                return res;
            } break;

            case SelectMode::XOR: {
                auto res = CCArray::create();
                for (auto obj : iterSelected(this)) {
                    if (!src->containsObject(obj)) {
                        res->addObject(obj);
                    }
                }
                for (auto obj : CCArrayExt<GameObject>(src)) {
                    if (!obj->m_isSelected && !res->containsObject(obj)) {
                        res->addObject(obj);
                    }
                }
                return res;
            } break;
        }
        return src;
    }

    CCArray* objectsInPolygon(std::vector<CCPoint> const& polygon) {
        auto res = CCArray::create();
        for (auto& obj : CCArrayExt<GameObject>(m_editorLayer->m_objects)) {
            if (polygonIntersect(polygon, { obj->getPosition() })) {
                res->addObject(obj);
            }
        }
        return res;
    }

    CCRect getSwipeRect(CCTouch* touch, CCEvent* event) {
        auto touchEnd = getTouchPoint(touch, event);
        auto start = m_editorLayer->m_objectLayer->convertToNodeSpace(m_swipeStart);
        auto end = m_editorLayer->m_objectLayer->convertToNodeSpace(touchEnd);
        if (end.x < start.x) {
            std::swap(start.x, end.x);
        }
        if (end.y < start.y) {
            std::swap(start.y, end.y);
        }
        end -= start;
        return { start, end };
    }

    bool isSwiping(CCTouch* touch, CCEvent* event) {
        auto touchEnd = getTouchPoint(touch, event);
        return 
            m_editorLayer->m_playbackMode != PlaybackMode::Playing &&
            m_selectedMode == 3 && m_swiping && (
                ccpDistance(m_swipeStart, touchEnd) > 2.f ||
                m_fields->select->getTool() == SelectTool::MagicWand
            );
    }

    CCArray* getSwipedObjects(CCTouch* touch, CCEvent* event) {
        auto touchEnd = getTouchPoint(touch, event);
        if (this->isSwiping(touch, event)) {
            auto _ = BlockAll();
            auto rect = this->getSwipeRect(touch, event);
            CCArray* objs;
            switch (m_fields->select->getTool()) {
                case SelectTool::Lasso: {
                    std::vector<CCPoint> casted;
                    for (auto& pt : m_fields->lassoPoints) {
                        casted.push_back(m_editorLayer->m_objectLayer->convertToNodeSpace(pt));
                    }
                    objs = this->objectsInPolygon(casted);
                } break;

                case SelectTool::MagicWand: {
                    objs = selectStructure(this, m_editorLayer->objectsAtPosition(rect.origin + rect.size));
                } break;

                case SelectTool::Swipe: default: {
                    objs = m_editorLayer->objectsInRect(rect, false);
                } break;
            }
            return this->filterSwipe(objs);
        }
        return nullptr;
    }

    void updateSwipePreview(CCTouch* touch, CCEvent* event) {
        for (auto obj : m_fields->deselection) {
            tintObject(obj, std::nullopt);
        }
        m_fields->deselection.clear();

        // reset colors
        for (auto obj : CCArrayExt<GameObject>(m_fields->selection)) {
            tintObject(obj, std::nullopt);
        }

        if (!Mod::get()->getSettingValue<bool>("selection-preview")) {
            m_fields->selection = nullptr;
            return;
        }
        
        auto objs = getSwipedObjects(touch, event);
        if (!objs) {
            m_fields->selection = nullptr;
            return;
        }

        for (auto obj : CCArrayExt<GameObject>(objs)) {
            if (!obj->m_isSelected) {
                tintObject(obj, {{ 155, 255, 155 }});
            }
        }
        for (auto obj : iterSelected(this)) {
            if (!objs->containsObject(obj)) {
                tintObject(obj, {{ 255, 155, 155 }});
                m_fields->deselection.push_back(obj);
            }
            else {
                tintObject(obj, {{ 0, 255, 0 }});
            }
        }

        m_fields->selection = objs;
    }

    bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
        if (m_fields->quickSelect) {
            m_fields->quickSelect->removeFromParent();
            m_fields->quickSelect = nullptr;
        }
        m_fields->lassoPoints = {};
        return EditorUI::ccTouchBegan(touch, event);
    }

    void ccTouchMoved(CCTouch* touch, CCEvent* event) {
        if (m_swiping && m_fields->select->getTool() == SelectTool::Lasso) {
            auto loc = touch->getLocation();
            if (m_fields->lassoPoints.empty() || ccpDistance(m_fields->lassoPoints.back(), loc) > 10.f) {
                m_fields->lassoPoints.push_back(loc);
            }
        }
        EditorUI::ccTouchMoved(touch, event);
        this->updateSwipePreview(touch, event);
    }

    void ccTouchEnded(CCTouch* touch, CCEvent* event) {
        for (auto obj : CCArrayExt<GameObject>(m_fields->selection)) {
            tintObject(obj, std::nullopt);
        }
        m_fields->selection = nullptr;
        for (auto obj : m_fields->deselection) {
            tintObject(obj, std::nullopt);
        }
        m_fields->deselection = {};
        if (auto objs = this->getSwipedObjects(touch, event)) {
            {
                auto _ = BlockAll();
                EditorUI::ccTouchEnded(touch, event);
                this->deselectAll();
            }
            this->selectObjects(objs, true);
        }
        else {
            EditorUI::ccTouchEnded(touch, event);
        }
    }

    void draw() {
        if (m_swiping) {
            switch (m_fields->select->getTool()) {
                case SelectTool::Swipe: {
                    EditorUI::draw();
                } break;

                case SelectTool::Lasso: {
                    CCNode::draw();
                    ccDrawColor4B(0, 255, 0, 255);
                    ccDrawPoly(m_fields->lassoPoints.data(), m_fields->lassoPoints.size(), true);
                } break;

                case SelectTool::MagicWand: {
                    CCNode::draw();
                } break;
            }
        }
        else {
            EditorUI::draw();
        }
    }

    void updateSwipeImage() {
        if (m_fields->select) {
            auto bspr = static_cast<ButtonSprite*>(m_swipeBtn->getNormalImage());
            bspr->m_subSprite->setDisplayFrame(CCSpriteFrameCache::get()->spriteFrameByName(
                getToolSprite(m_fields->select->getTool())
            ));
            if (auto c = bspr->getChildByID("select-mode"_spr)) {
                c->removeFromParent();
            }
            auto mode = CCSprite::createWithSpriteFrameName(
                getModeSprite(m_fields->select->getMode())
            );
            mode->setID("select-mode"_spr);
            mode->setScale(.4f);
            mode->setZOrder(15);
            mode->setPosition({ 11.f, 11.f });
            bspr->addChild(mode);
        }
    }

    void toggleSwipe(CCObject* sender) {
        EditorUI::toggleSwipe(sender);
        if (m_fields->select) {
            m_fields->select->updateTool();
            this->updateSwipeImage();
            if (
                !m_fields->quickSelect && 
                duration_cast<milliseconds>(Clock::now() - m_fields->lastSwipeClick).count() < 250
            ) {
                auto quick = QuickSelect::create(this);
                quick->setPosition(m_swipeBtn->getParent()->convertToWorldSpace(
                    m_swipeBtn->getPosition()
                ) + ccp(0, 85));
                quick->setID("quick-select"_spr);
                m_fields->quickSelect = quick;
                this->addChild(quick);
                this->toggleMode(m_editModeBtn);
            }
            m_fields->lastSwipeClick = Clock::now();
        }
    }
};

void ASelect::updateEditorUI() {
    static_cast<SelectUI*>(m_ui)->updateSwipeImage();
}

void BetterSelect::updateEditorUI() {
    ASelect::updateEditorUI();
    auto select = static_cast<SelectUI*>(m_ui)->m_fields->quickSelect;
    if (select) {
        select->updateTool();
        select->updateMode();
    }
}

void QuickSelect::updateEditorUI() {
    ASelect::updateEditorUI();
    auto select = static_cast<SelectUI*>(m_ui)->m_fields->select;
    if (select) {
        select->updateTool();
        select->updateMode();
    }
}
