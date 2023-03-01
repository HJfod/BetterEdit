
#include <Geode/modify/EditorUI.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <script/Parser.hpp>

USE_GEODE_NAMESPACE();

class $modify(ScriptingUI, EditorUI) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;
        
        auto menu = this->getChildByID("editor-buttons-menu");
        if (menu) {
            for (auto& file : file::readDirectory("C:/Users/HJfod/Documents/github/GeodeSDK/BetterEdit/scripts")
                .unwrapOr(std::vector<ghc::filesystem::path> {})
            ) {
                auto button = CCMenuItemSpriteExtra::create(
                    EditorButtonSprite::create(
                        CCLabelBMFont::create(file.filename().string().c_str(), "bigFont.fnt"),
                        EditorBaseColor::Orange
                    ),
                    this, menu_selector(ScriptingUI::onScript)
                );
                button->setUserObject(CCString::create(file.string()));
                menu->addChild(button);
                menu->updateLayout();
            }
        }

        return true;
    }

    void onScript(CCObject* sender) {
        auto path = ghc::filesystem::path(static_cast<CCString*>(
            static_cast<CCMenuItemSpriteExtra*>(sender)->getUserObject()
        )->getCString());
        auto res = script::State::run(path, true);
        if (!res) {
            log::error("Error running script {}: {}", path, res.unwrapErr());
        }
    }
};
