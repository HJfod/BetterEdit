
#include <Geode/modify/EditorUI.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/ui/Notification.hpp>
#include <Geode/ui/InputNode.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/General.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/TextInputDelegate.hpp>
#include <Geode/binding/GameToolbox.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/TextArea.hpp>
#include <script/Parser.hpp>

using namespace geode::prelude;

using script::Rc;
using script::State;
using script::Lit;
using script::NumLit;
using script::StrLit;
using script::BoolLit;

class ScriptingUI;

static std::vector<std::pair<std::string, bool>> SCRIPT_LOGS = {};

static void scriptLog(std::string const& msg, bool error) {
    SCRIPT_LOGS.emplace_back(msg, error);
    if (error) {
        log::error("{}", msg);
    }
    else {
        log::info("{}", msg);
    }
}

static void runScript(Rc<State> state) {
    state->setLogger([](auto const& msg) { scriptLog(msg, false); });
    auto eval = state->run();
    if (!eval) {
        scriptLog(fmt::format("Error running script: {}", eval.unwrapErr()), true);
        Notification::create(
            "Error running script",
            NotificationIcon::Error
        )->show();
    }
}

class ScriptInputPopup : public Popup<Rc<State>>, TextInputDelegate {
protected:
    Rc<State> m_state;

    static const char* placeholderForType(Lit const& lit) {
        return std::visit(makeVisitor {
            [&](NumLit const&) {
                return "Number";
            },
            [&](StrLit const&) {
                return "String";
            },
            [&](BoolLit const&) {
                return "Boolean";
            },
            [&](auto const&) {
                return "Value";
            },
        }, lit);
    }

    static const char* filterForType(Lit const& lit) {
        return std::visit(makeVisitor {
            [&](NumLit const&) {
                return "0123456789+- ";
            },
            [&](auto const&) {
                return "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_;,:.+?\\/&%$@\"'*~^!<>=[](){}";
            },
        }, lit);
    }

    bool setup(Rc<State> state) override {
        m_state = state;
        m_noElasticity = true;

        this->setTitle(state->attrs.title.value_or("Run Script"));

        auto winSize = CCDirector::get()->getWinSize();

        auto contentSize = m_size - CCSize { 20.f, 75.f };
        auto content = CCNode::create();
        content->setContentSize(contentSize);
        content->setAnchorPoint({ .5f, .5f });

        for (auto& param : state->attrs.parameters) {
            // apple clang just straight up doesn't work with auto& [var, def, title]
            auto& var = std::get<0>(param);
            auto& def = std::get<1>(param);
            auto& title = std::get<2>(param);
            auto menu = CCMenu::create();
            menu->setContentSize({ contentSize.width / 2, 50 });
            menu->ignoreAnchorPointForPosition(false);

            std::visit(makeVisitor {
                [&](BoolLit& value) {
                    GameToolbox::createToggleButton(
                        title.c_str(), menu_selector(ScriptInputPopup::onToggle), value, 
                        menu, winSize / 2 + CCPoint {
                            -menu->getContentSize().width / 2 + 20.f, 0.f
                        }, this, menu, .7f, .5f, menu->getContentSize().width - 40.f,
                        { 5.f, 0.f }, nullptr, false, 0, nullptr
                    )->setUserObject(CCString::create(var));
                },
                [&](NumLit& value) {
                    auto label = CCLabelBMFont::create(title.c_str(), "bigFont.fnt");
                    label->setPosition(
                        menu->getContentSize().width / 2, 
                        menu->getContentSize().height - 10.f 
                    );
                    label->limitLabelWidth(menu->getContentSize().width - 10.f, .5f, .1f);
                    menu->addChild(label);

                    auto input = InputNode::create(60.f, placeholderForType(def));
                    input->getInput()->setAllowedChars("0123456789+-. ");
                    input->setPosition(
                        menu->getContentSize().width / 2, 
                        menu->getContentSize().height / 2 - 10.f 
                    );
                    input->getInput()->setUserObject(CCString::create(var));
                    input->getInput()->setDelegate(this);
                    input->setScale(.85f);
                    input->setString(script::tokenToString(def));
                    menu->addChild(input);
                },
                [&](auto const&) {
                    auto info = CCLabelBMFont::create("Unsupported", "goldFont.fnt");
                    info->limitLabelWidth(menu->getContentSize().width, .5f, .1f);
                    info->setPosition(menu->getContentSize() / 2);
                    menu->addChild(info);
                },
            }, def);

            content->addChild(menu);
        }

        content->setLayout(
            RowLayout::create()
                ->setGap(0.f)
                ->setCrossAxisOverflow(false)
                ->setGrowCrossAxis(true)
        );
        content->setPosition(winSize / 2);
        this->addChild(content);

        auto runBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Run"),
            this, menu_selector(ScriptInputPopup::onRun)
        );
        runBtn->setPosition(0.f, -m_size.height / 2 + 25.f);
        m_buttonMenu->addChild(runBtn);

        return true;
    }

    void onToggle(CCObject* sender) {
        auto toggle = static_cast<CCMenuItemToggler*>(sender);
        auto var = static_cast<CCString*>(toggle->getUserObject());
        m_state->get(var->getCString())->value = !toggle->isToggled();
    }

    void textChanged(CCTextInputNode* input) override {
        auto varName = static_cast<CCString*>(input->getUserObject());
        auto var = m_state->get(varName->getCString());
        std::visit(makeVisitor {
            [&](NumLit const&) {
                if (input->getString()) {
                    try {
                        auto val = std::stod(input->getString());
                        var->value =val;
                    }
                    catch(...) {}
                }
            },
            [&](StrLit const&) {
                if (input->getString()) {
                    var->value = input->getString();
                }
            },
            [&](auto const&) {},
        }, var->value);
    }

    void onRun(CCObject*) {
        this->run();
    }

    void run() {
        this->onClose(nullptr);
        runScript(m_state);
    } 

    static CCSize sizeForInputs(size_t count) {
        if (count < 2) {
            return { 200.f, 150.f };
        }
        if (count < 5) {
            return { 280.f, 180.f };
        }
        if (count < 9) {
            return { 320.f, 240.f };
        }
        return { 380.f, 280.f };
    }

public:
    static ScriptInputPopup* create(Rc<State> state) {
        auto size = sizeForInputs(state->attrs.parameters.size());
        auto ret = new ScriptInputPopup();
        if (ret && ret->init(size.width, size.height, state)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

class ScriptsPopup : public Popup<> {
protected:
    ScrollLayer* m_list;
    CCNode* m_listBorders;

    bool setup() override {
        m_noElasticity = true;

        this->setTitle("Run Script");

        auto winSize = CCDirector::get()->getWinSize();

        auto listSize = CCSize { 360.f, 200.f };
        m_list = ScrollLayer::create(listSize);
        bool hasBG = true;
        for (auto& script : scripts()) {
            auto menuSize = CCSize { listSize.width, 35.f };
            auto menu = CCMenu::create();
            menu->ignoreAnchorPointForPosition(false);
            menu->setContentSize({ listSize.width, menuSize.height });
            if (hasBG) {
                auto bg = CCLayerColor::create({ 0, 0, 0, 95 }, listSize.width, menuSize.height);
                menu->addChild(bg);
            }
            hasBG ^= 1;

            auto label = CCLabelBMFont::create(State::name(script).c_str(), "bigFont.fnt");
            label->setAnchorPoint({ .0f, .5f });
            label->setPosition(15.f, menuSize.height / 2);
            label->limitLabelWidth(260.f, .5f, .1f);
            menu->addChild(label);

            auto runSpr = EditorButtonSprite::createWithSpriteFrameName("run-script.png"_spr, .7f);
            runSpr->setScale(.85f);
            auto runBtn = CCMenuItemSpriteExtra::create(
                runSpr, this, menu_selector(ScriptsPopup::onRun)
            );
            runBtn->setAttribute("script-path"_spr, script);
            runBtn->setPosition(listSize.width - menuSize.height / 2 - 5.f, menuSize.height / 2);
            menu->addChild(runBtn);

            m_list->m_contentLayer->addChild(menu);
        }
        m_list->m_contentLayer->setLayout(
            ColumnLayout::create()
                ->setGap(0.f)
                ->setAxisReverse(true)
        );
        m_list->setPosition(winSize / 2 - listSize / 2 - CCPoint { 0.f, 10.f });
        this->addChild(m_list);

        m_listBorders = CCNode::create();
        addListBorders(m_listBorders, winSize / 2 - CCPoint { 0.f, 10.f }, listSize);
        this->addChild(m_listBorders);

        auto logsBtnOnSpr = ButtonSprite::create(
            "Logs", "bigFont.fnt", "GJ_button_02.png", .6f
        );
        logsBtnOnSpr->setScale(.6f);

        auto logsBtnOffSpr = ButtonSprite::create(
            "Logs", "bigFont.fnt", "GJ_button_05.png", .6f
        );
        logsBtnOffSpr->setScale(.6f);

        auto logsBtn = CCMenuItemToggler::create(
            logsBtnOffSpr, logsBtnOnSpr,
            this, menu_selector(ScriptsPopup::onViewLogs)
        );
        logsBtn->setPosition(m_size.width / 2 - 40.f, m_size.height / 2 - 25.f);
        m_buttonMenu->addChild(logsBtn);

        return true;
    }

    void onViewLogs(CCObject* sender) {
        if (!static_cast<CCMenuItemToggler*>(sender)->isToggled()) {
            m_list->setVisible(false);

            auto winSize = CCDirector::get()->getWinSize();

            auto listSize = CCSize { 360.f, 200.f };
            auto list = ScrollLayer::create(listSize);
            bool hasBG = true;
            for (auto& log : SCRIPT_LOGS) {
                auto menuSize = CCSize { listSize.width, 35.f };
                auto menu = CCMenu::create();
                menu->ignoreAnchorPointForPosition(false);
                menu->setContentSize({ listSize.width, menuSize.height });
                if (hasBG || log.second) {
                    auto bg = CCLayerColor::create(
                        (log.second ? ccColor4B { 255, 0, 0, 95 } : ccColor4B { 0, 0, 0, 95 }),
                        listSize.width, menuSize.height
                    );
                    menu->addChild(bg);
                }
                hasBG ^= 1;

                auto label = TextArea::create(
                    log.first, "chatFont.fnt", 1.f, listSize.width - 30.f, { .5f, .5f }, 20.f, true
                );
                label->setPosition(menuSize / 2);
                limitNodeSize(label, menuSize, 1.f, .1f);
                menu->addChild(label);

                list->m_contentLayer->addChild(menu);
            }
            list->m_contentLayer->setLayout(
                ColumnLayout::create()
                    ->setGap(0.f)
                    ->setAxisReverse(true)
            );
            list->setPosition(winSize / 2 - listSize / 2 - CCPoint { 0.f, 10.f });
            this->addChild(list);
        }
        else {
            m_list->setVisible(true);
        }
    }

    void onRun(CCObject* sender) {
        auto path = static_cast<CCMenuItemSpriteExtra*>(sender)
            ->template getAttribute<ghc::filesystem::path>("script-path"_spr);
        if (!path) return;

        this->onClose(nullptr);

        auto res = script::State::parse(path.value(), true);
        if (!res) {
            scriptLog(fmt::format(
                "Error parsing script {}: {}",
                path.value().generic_string(), res.unwrapErr()
            ), true);
            Notification::create(
                "Error parsing script",
                NotificationIcon::Error
            )->show();
            return;
        }
        auto state = res.unwrap();
        if (state->attrs.parameters.size()) {
            ScriptInputPopup::create(state)->show();
        }
        else {
            runScript(state);
        }
    }

public:
    static ScriptsPopup* create() {
        auto ret = new ScriptsPopup;
        if (ret && ret->init(400.f, 280.f)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    static std::vector<ghc::filesystem::path> scripts() {
        std::vector<ghc::filesystem::path> res;
        for (auto& dir : {
            Mod::get()->getResourcesDir(),
            Mod::get()->getConfigDir() / "scripts"
        }) {
            if (auto files = file::readDirectory(dir)) {
                for (auto& file : files.unwrap()) {
                    if (file.extension() == ".swipe") {
                        res.push_back(file);
                    }
                }
            }
        }
        return res;
    }
};

class $modify(ScriptingUI, EditorUI) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;
        
        auto menu = this->getChildByID("editor-buttons-menu");
        if (menu) {
            auto spr = EditorButtonSprite::createWithSpriteFrameName(
                "run-script.png"_spr, .7f, EditorBaseColor::Orange
            );
            auto button = CCMenuItemSpriteExtra::create(
                spr, this, menu_selector(ScriptingUI::onScripts)
            );
            button->setSizeMult(1.2f);
            button->setContentSize({ 40.f, 40.f });
            menu->addChild(button);
            menu->updateLayout();
        }

        return true;
    }

    void onScripts(CCObject*) {
        ScriptsPopup::create()->show();
    }
};
