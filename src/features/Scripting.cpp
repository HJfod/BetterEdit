
#include <Geode/modify/EditorUI.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/ui/Notification.hpp>
#include <Geode/ui/InputNode.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <script/Parser.hpp>

USE_GEODE_NAMESPACE();

using script::Rc;
using script::State;
using script::Lit;
using script::NumLit;
using script::StrLit;
using script::BoolLit;

class ScriptingUI;

class ScriptInputPopup : public Popup<Rc<State>, ScriptingUI*>, TextInputDelegate {
protected:
    ScriptingUI* m_ui;
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

    bool setup(Rc<State> state, ScriptingUI* ui) override {
        m_ui = ui;
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

    void onRun(CCObject*);

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
    static ScriptInputPopup* create(Rc<State> state, ScriptingUI* ui) {
        auto size = sizeForInputs(state->attrs.parameters.size());
        auto ret = new ScriptInputPopup();
        if (ret && ret->init(size.width, size.height, state, ui)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

static std::vector<ghc::filesystem::path> readDirs(std::vector<ghc::filesystem::path> const& dirs) {
    std::vector<ghc::filesystem::path> res;
    for (auto& dir : dirs) {
        ranges::push(res, file::readDirectory(dir)
            .unwrapOr(std::vector<ghc::filesystem::path> {})
        );
    }
    return res;
}

class $modify(ScriptingUI, EditorUI) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;
        
        auto menu = this->getChildByID("editor-buttons-menu");
        if (menu) {
            for (auto& file : readDirs({
                Mod::get()->getResourcesDir(),
                Mod::get()->getConfigDir() / "scripts"
            })) {
                if (file.extension() == ".swipe") {
                    auto spr = EditorButtonSprite::create(
                        CCLabelBMFont::create(file.filename().string().c_str(), "bigFont.fnt"),
                        EditorBaseColor::Orange
                    );
                    auto button = CCMenuItemSpriteExtra::create(
                        spr, this, menu_selector(ScriptingUI::onScript)
                    );
                    button->setSizeMult(1.2f);
                    button->setContentSize({ 40.f, 40.f });
                    button->setUserObject(CCString::create(file.string()));
                    menu->addChild(button);
                    menu->updateLayout();
                }
            }
        }

        return true;
    }

    void onScript(CCObject* sender) {
        auto path = ghc::filesystem::path(static_cast<CCString*>(
            static_cast<CCMenuItemSpriteExtra*>(sender)->getUserObject()
        )->getCString());
        auto res = script::State::parse(path, true);
        if (!res) {
            log::error("Error parsing script {}: {}", path, res.unwrapErr());
            Notification::create(
                "Error parsing script",
                NotificationIcon::Error
            )->show();
            return;
        }
        auto state = res.unwrap();

        if (state->attrs.parameters.size()) {
            ScriptInputPopup::create(state, this)->show();
        }
        else {
            this->run(state);
        }
    }

    void run(Rc<State> state) {
        auto eval = state->run();
        if (!eval) {
            log::error("Error running script: {}", eval.unwrapErr());
            Notification::create(
                "Error running script",
                NotificationIcon::Error
            )->show();
        }
    } 
};

void ScriptInputPopup::onRun(CCObject*) {
    m_ui->run(m_state);
    this->onClose(nullptr);
}
