#include "ScriptingUI.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/ui/TextArea.hpp>
#include <Geode/ui/Notification.hpp>
#include <Geode/utils/ColorProvider.hpp>
#include <utils/Editor.hpp>

using namespace geode::prelude;

$execute {
    ColorProvider::get()->define("script-log-error"_spr,   ccc4(245, 60,  35,  60));
    ColorProvider::get()->define("script-log-warning"_spr, ccc4(245, 207, 35,  90));
    ColorProvider::get()->define("script-log-info"_spr,    ccc4(255, 255, 255, 30));
    ColorProvider::get()->define("script-log-status"_spr,  ccc4(131, 107, 235, 60));
}

class RunScriptPopup;

bool ScriptNode::init(RunScriptPopup* popup, std::shared_ptr<JsScript> script, float width) {
    if (!CCNode::init())
        return false;
    
    m_popup = popup;
    m_script = script;
    this->setContentSize(ccp(width, 35));

    m_bg = CCScale9Sprite::create("square02b_small.png");
    m_bg->setContentSize(m_obContentSize - ccp(3, 3));
    this->addChildAtPosition(m_bg, Anchor::Center);

    auto title = CCLabelBMFont::create("", "bigFont.fnt");
    this->addChildAtPosition(title, Anchor::Left, ccp(5, 7), ccp(0, .5f));

    auto creator = CCLabelBMFont::create("", "goldFont.fnt");
    this->addChildAtPosition(creator, Anchor::Left, ccp(5, -7), ccp(0, .5f));

    auto menu = CCMenu::create();

    m_runSpr = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
    m_runSpr->setScale(.2f);
    m_runBtn = CCMenuItemSpriteExtra::create(m_runSpr, this, menu_selector(ScriptNode::onRun));
    menu->addChild(m_runBtn);

    auto logsSprOff = CircleButtonSprite::createWithSpriteFrameName(
        "eye-on.png"_spr, 1.f, CircleBaseColor::Green
    );
    logsSprOff->setScale(.35f);

    auto logsSprOn = CircleButtonSprite::createWithSpriteFrameName(
        "eye-on.png"_spr, 1.f, CircleBaseColor::Cyan
    );
    logsSprOn->setScale(.35f);

    m_logsToggle = CCMenuItemToggler::create(
        logsSprOff, logsSprOn, this, menu_selector(ScriptNode::onLogs)
    );
    m_logsToggle->m_notClickable = true;
    menu->addChild(m_logsToggle);

    menu->setLayout(RowLayout::create()->setAxisReverse(true)->setAxisAlignment(AxisAlignment::End));
    this->addChildAtPosition(menu, Anchor::Right, ccp(-10, 0), ccp(1, .5f));

    m_logsLabel = CCLabelBMFont::create("0 Logs", "chatFont.fnt");
    m_logsLabel->setScale(.25f);
    m_logsLabel->setColor(ccBLACK);
    this->addChildAtPosition(m_logsLabel, Anchor::Bottom, ccp(0, 5));

    title->setString(m_script->getTitle().c_str());
    title->limitLabelWidth(width - 65, .4f, .1f);

    creator->setString(m_script->getAuthor().c_str());
    creator->limitLabelWidth(width - 65, .4f, .1f);

    m_selectionSprite = CCScale9Sprite::create("GJ_square07.png");
    m_selectionSprite->setContentSize(m_obContentSize / .7f - ccp(1, 1));
    m_selectionSprite->setColor(ccc3(0, 255, 255));
    m_selectionSprite->setScale(.7f);
    this->addChildAtPosition(m_selectionSprite, Anchor::Center, ccp(0, 0));

    m_logListener = JsScriptLoggedEvent(m_script).listen([this]() {
        this->updateState();
    });

    this->updateState();

    return true;
}

void ScriptNode::onRun(CCObject*) {
    m_popup->view(m_script);
    m_script->run();
}
void ScriptNode::onLogs(CCObject*) {
    m_popup->view(m_script);
}

ScriptNode* ScriptNode::create(RunScriptPopup* popup, std::shared_ptr<JsScript> script, float width) {
    auto ret = new ScriptNode();
    if (ret && ret->init(popup, script, width)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void ScriptNode::updateState() {
    bool selected = m_popup->s_selected.lock() == m_script;

    bool canRun = m_script->canRun();
    m_runSpr->setColor(canRun ? ccWHITE : ccGRAY);
    m_runSpr->setOpacity(canRun ? 255 : 155);
    m_runBtn->setEnabled(canRun);

    m_selectionSprite->setVisible(selected);

    ccColor4B color;
    switch (m_script->getLastRunSeverity()) {
        case JsScript::Log::Level::Warning: {
            color = "script-log-warning"_cc4b;
        } break;

        case JsScript::Log::Level::Error: {
            color = "script-log-error"_cc4b;
        } break;

        default: {
            color = to4B(ccBLACK, 90);
        } break;
    }
    m_bg->setColor(to3B(color));
    m_bg->setOpacity(color.a);

    m_logsToggle->toggle(selected);
    m_logsLabel->setString(fmt::format("{} Logs", m_script->getLastRunLogs().size()).c_str());
}

std::weak_ptr<JsScript> RunScriptPopup::s_selected = std::weak_ptr<JsScript>();

bool RunScriptPopup::init() {
    if (!Popup::init(380, 280))
        return false;

    m_noElasticity = true;
    this->setTitle("Run Script");

    m_list = ScrollLayer::create(ccp(175, 205));

    auto listBG = CCScale9Sprite::create("square02b_small.png");
    listBG->setContentSize(m_list->getContentSize() + ccp(5, 5));
    listBG->setColor(ccBLACK);
    listBG->setOpacity(150);
    m_mainLayer->addChildAtPosition(listBG, Anchor::Left, ccp(m_list->getContentWidth() / 2 + 10, 0));

    m_list->m_contentLayer->setLayout(
        ColumnLayout::create()
            ->setAutoGrowAxis(m_list->getContentHeight())
            ->setAxisAlignment(AxisAlignment::End)
            ->setAxisReverse(true)
            ->setGap(2.5f)
    );
    m_mainLayer->addChildAtPosition(m_list, Anchor::Left, ccp(10, -m_list->getContentHeight() / 2));

    m_logsList = ScrollLayer::create(ccp(170, 200));

    auto logsBG = CCScale9Sprite::create("square02b_small.png");
    logsBG->setContentSize(m_logsList->getContentSize() + ccp(10, 10));
    logsBG->setColor(ccBLACK);
    logsBG->setOpacity(150);
    m_mainLayer->addChildAtPosition(logsBG, Anchor::Right, ccp(-m_logsList->getContentWidth() / 2 - 10, 0));

    m_logsList->m_contentLayer->setLayout(
        ColumnLayout::create()
            ->setAutoGrowAxis(m_logsList->getContentHeight())
            ->setAxisAlignment(AxisAlignment::End)
            ->setAxisReverse(true)
    );
    m_mainLayer->addChildAtPosition(
        m_logsList, Anchor::Right,
        ccp(-m_logsList->getContentWidth() - 10, -m_logsList->getContentHeight() / 2)
    );

    auto reloadSpr = CCSprite::createWithSpriteFrameName("GJ_replayBtn_001.png");
    reloadSpr->setScale(.35f);
    auto reloadBtn = CCMenuItemSpriteExtra::create(
        reloadSpr, this, menu_selector(RunScriptPopup::onReload)
    );
    m_buttonMenu->addChildAtPosition(reloadBtn, Anchor::BottomLeft, ccp(20, 20));

    // On first boot try reloading scripts fully
    static bool LOADED_SCRIPTS = false;
    if (!LOADED_SCRIPTS) {
        LOADED_SCRIPTS = true;
        this->runAction(CallFuncExt::create([this] {
            this->onReload(nullptr);
        }));
    }
    // Otherwise just setup the list
    else {
        this->reloadList();
    }

    return true;
}

void RunScriptPopup::reloadList() {
    m_list->m_contentLayer->removeAllChildren();
    auto scripts = ScriptManager::get()->getScripts();
    for (auto script : scripts) {
        m_list->m_contentLayer->addChild(ScriptNode::create(this, script, m_list->getContentWidth()));
    }
    if (auto selected = s_selected.lock()) {
        this->view(selected);
    }
    else if (scripts.size()) {
        this->view(scripts.front());
    }
    m_list->m_contentLayer->updateLayout();
}
void RunScriptPopup::updateLogs() {
    m_logsList->m_contentLayer->removeAllChildren();
    auto selected = s_selected.lock();
    if (!selected) {
        auto label = CCLabelBMFont::create("Select a script to run", "bigFont.fnt");
        label->setLayoutOptions(AxisLayoutOptions::create()->setScaleLimits(.1f, .5f));
        m_logsList->m_contentLayer->addChild(label);
    }
    else for (auto& log : selected->getLastRunLogs()) {
        auto bg = CCScale9Sprite::create("square02b_small.png");
        bg->setContentWidth(m_logsList->getContentWidth() - 5);

        auto label = SimpleTextArea::create(log.message, "chatFont.fnt", .5f);
        label->setWidth(bg->getContentWidth() - 10);
        label->setAlignment(kCCTextAlignmentLeft);
        label->setAnchorPoint({ 0, .5f });
        bg->setContentHeight(label->getContentHeight() + 10);
        bg->addChildAtPosition(label, Anchor::Left, ccp(5, 0));
        
        ccColor4B color;
        switch (log.level) {
            case JsScript::Log::Level::Status: {
                color = "script-log-status"_cc4b;
            } break;

            case JsScript::Log::Level::Info: {
                color = "script-log-info"_cc4b;
            } break;

            case JsScript::Log::Level::Warning: {
                color = "script-log-warning"_cc4b;
            } break;

            case JsScript::Log::Level::Error: {
                color = "script-log-error"_cc4b;
            } break;
        }
        bg->setColor(to3B(color));
        bg->setOpacity(color.a);
        m_logsList->m_contentLayer->addChild(bg);
    }
    m_logsList->m_contentLayer->updateLayout();
}

void RunScriptPopup::onReload(CCObject* sender) {
    s_selected.reset();
    ScriptManager::get()->reloadScripts();
    this->reloadList();
    if (sender) {
        Notification::create("Reloaded scripts", NotificationIcon::Success)->show();
    }
}

RunScriptPopup* RunScriptPopup::create() {
    auto ret = new RunScriptPopup();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void RunScriptPopup::view(std::shared_ptr<JsScript> script) {
    s_selected = script;
    this->updateLogs();
    m_logListener = JsScriptLoggedEvent(script).listen([this] {
        this->updateLogs();
    });
    for (auto node : CCArrayExt<ScriptNode*>(m_list->m_contentLayer->getChildren())) {
        node->updateState();
    }
}

class $modify(ScriptingUI, EditorUI) {
    struct Fields final {
        ListenerHandle onUIHide;
    };

    $override
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;

        // todo: stabilize feature
        if (!Mod::get()->getSettingValue<bool>("run-script-button")) {
            return true;
        }

        if (auto menu = this->querySelector("editor-buttons-menu")) {
            auto spr = EditorButtonSprite::createWithSpriteFrameName("script.png"_spr, .85f, EditorBaseColor::LightBlue);
            auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ScriptingUI::onScripts));
            btn->setContentSize(ccp(40, 40));
            menu->addChild(btn);
            menu->updateLayout();

            m_fields->onUIHide = UIShowEvent(this).listen([btn](bool show) {
                btn->setVisible(show);
            });
        }
        
        return true;
    }

    void onScripts(CCObject*) {
        RunScriptPopup::create()->show();
    }
};
