#include <MoreTabs.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <other/Utils.hpp>
#include <tracking/Tracking.hpp>

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

enum class SelectType {
    Unique,
    Add,
    Subtract,
    Intersect,
    XOR,
};

static const char* getTypeSprite(SelectType type) {
    switch (type) {
        default:
        case SelectType::Unique: return "select-unique.png"_spr;
        case SelectType::Add: return "select-add.png"_spr;
        case SelectType::Subtract: return "select-subtract.png"_spr;
        case SelectType::Intersect: return "select-intersect.png"_spr;
        case SelectType::XOR: return "select-xor.png"_spr;
    }
}

class BetterSelect : public CCNode {
protected:
    EditorUI* m_ui;
    CCMenu* m_selectToolMenu;
    CCMenu* m_selectTypeMenu;
    std::vector<CCMenuItemToggler*> m_tools;
    std::vector<CCMenuItemToggler*> m_types;
    SelectTool m_tool = SelectTool::Swipe;
    SelectType m_type = SelectType::Add;

    bool init(EditorUI* ui) {
        if (!CCNode::init())
            return false;
        
        m_ui = ui;

        auto winSize = CCDirector::get()->getWinSize();

        m_selectToolMenu = CCMenu::create();
        m_selectToolMenu->setContentSize({ 110.f, 50.f });
        m_selectToolMenu->setPosition(winSize.width / 2 - 70.f, 45.f);
        m_selectToolMenu->ignoreAnchorPointForPosition(false);

        this->addTool(SelectTool::Swipe);
        this->addTool(SelectTool::Lasso);
        this->addTool(SelectTool::MagicWand);
        
        m_selectToolMenu->setLayout(
            RowLayout::create()
                ->setGrowCrossAxis(true)
                ->setAxisAlignment(AxisAlignment::End)
        );
        this->addChild(m_selectToolMenu);

        m_selectTypeMenu = CCMenu::create();
        m_selectTypeMenu->setContentSize({ 110.f, 50.f });
        m_selectTypeMenu->setPosition(winSize.width / 2 + 70.f, 45.f);
        m_selectTypeMenu->ignoreAnchorPointForPosition(false);

        this->addType(SelectType::Unique);
        this->addType(SelectType::Add);
        this->addType(SelectType::Subtract);
        this->addType(SelectType::Intersect);
        this->addType(SelectType::XOR);
        
        m_selectTypeMenu->setLayout(
            RowLayout::create()
                ->setGrowCrossAxis(true)
                ->setAxisAlignment(AxisAlignment::Start)
        );
        this->addChild(m_selectTypeMenu);

        this->updateTool();
        this->updateType();

        return true;
    }

    CCMenuItemToggler* addTool(SelectTool to) {
        auto tool = createEditorButton(
            getToolSprite(to), this, menu_selector(BetterSelect::onSelectTool)
        );
        tool->setTag(static_cast<int>(to));
        m_tools.push_back(tool);
        m_selectToolMenu->addChild(tool);
        return tool;
    }

    CCMenuItemToggler* addType(SelectType ty) {
        auto type = createEditorButton(
            getTypeSprite(ty), this, menu_selector(BetterSelect::onSelectType)
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
                m_tool = static_cast<SelectTool>(tool->getTag());
                // this code makes no sense until you remember that 
                // CCMenuItemToggler toggles after the callback
                m_ui->m_swipeEnabled = tool->isToggled();
                m_ui->toggleSwipe(nullptr);
                tool->toggle(!m_ui->m_swipeEnabled);
            }
        }
    }

    void onSelectType(CCObject* sender) {
        for (auto& type : m_types) {
            type->toggle(false);
            if (type == sender) {
                m_type = static_cast<SelectType>(type->getTag());
            }
        }
    }

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

    void updateTool() {
        for (auto& tool : m_tools) {
            tool->toggle(
                m_ui->m_swipeEnabled &&
                static_cast<SelectTool>(tool->getTag()) == m_tool
            );
        }
    }

    void updateType() {
        for (auto& type : m_types) {
            type->toggle(
                static_cast<SelectType>(type->getTag()) == m_type
            );
        }
    }

    SelectTool getTool() const {
        return m_tool;
    }

    SelectType getType() const {
        return m_type;
    }
};

class $modify(EditorUI) {
    BetterSelect* select = nullptr;
    std::vector<CCPoint> lassoPoints;

    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;

        MoreTabs::get(this)->addEditTab(
            "edit_swipeBtn_001.png",
            m_fields->select = BetterSelect::create(this)
        );

        return true;
    }

    CCArray* filterSwipe(CCArray* src) {
        switch (m_fields->select->getType()) {
            case SelectType::Unique: {
                return src;
            } break;

            case SelectType::Add: {
                src->addObjectsFromArray(this->getSelectedObjects());
                return src;
            } break;

            case SelectType::Subtract: {
                auto res = CCArray::create();
                for (auto obj : iterSelected(this)) {
                    if (!src->containsObject(obj)) {
                        res->addObject(obj);
                    }
                }
                return res;
            } break;

            case SelectType::Intersect: {
                auto res = CCArray::create();
                for (auto obj : iterSelected(this)) {
                    if (src->containsObject(obj)) {
                        res->addObject(obj);
                    }
                }
                return res;
            } break;

            case SelectType::XOR: {
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

    bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
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
    }

    void ccTouchEnded(CCTouch* touch, CCEvent* event) {
        auto touchEnd = getTouchPoint(touch, event);
        if (
            m_editorLayer->m_playbackMode != PlaybackMode::Playing &&
            m_selectedMode == 3 && m_swiping && 
            ccpDistance(m_swipeStart, touchEnd) > 2.f
        ) {
            auto start = m_editorLayer->m_objectLayer->convertToNodeSpace(m_swipeStart);
            auto end = m_editorLayer->m_objectLayer->convertToNodeSpace(touchEnd);
            if (end.x < start.x) {
                std::swap(start.x, end.x);
            }
            if (end.y < start.y) {
                std::swap(start.y, end.y);
            }
            end -= start;
            CCArray* objs;
            if (m_fields->select->getTool() == SelectTool::Lasso) {
                std::vector<CCPoint> casted;
                for (auto& pt : m_fields->lassoPoints) {
                    casted.push_back(m_editorLayer->m_objectLayer->convertToNodeSpace(pt));
                }
                objs = this->objectsInPolygon(casted);
            } else {
                objs = m_editorLayer->objectsInRect(CCRect { start, end }, false);
            }
            auto filtered = filterSwipe(objs);
            {
                auto _ = BlockAll();
                EditorUI::ccTouchEnded(touch, event);
                this->deselectAll();
            }
            this->selectObjects(filtered, true);
        }
        else {
            EditorUI::ccTouchEnded(touch, event);
        }
    }

    void draw() {
        if (m_swiping && m_fields->select->getTool() == SelectTool::Lasso) {
            CCNode::draw();
            ccDrawColor4B(0, 255, 0, 255);
            ccDrawPoly(m_fields->lassoPoints.data(), m_fields->lassoPoints.size(), true);
        }
        else {
            EditorUI::draw();
        }
    }

    void toggleSwipe(CCObject* sender) {
        EditorUI::toggleSwipe(sender);
        if (m_fields->select) {
            m_fields->select->updateTool();
            auto bspr = static_cast<ButtonSprite*>(m_swipeBtn->getNormalImage());
            bspr->m_subSprite->setDisplayFrame(CCSpriteFrameCache::get()->spriteFrameByName(
                getToolSprite(m_fields->select->getTool())
            ));
        }
    }
};
