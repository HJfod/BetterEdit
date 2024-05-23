#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

using namespace geode::prelude;

struct NextFreeLayer : Modify<NextFreeLayer, EditorUI> {
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) {
            return false;
        }

        auto layer_btn_mnu = this->getChildByIDRecursive("layer-menu");
        auto next_free_spr = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
        auto new_layer_pos = layer_btn_mnu->getPositionX() - 10.f;
        next_free_spr->setScale(0.54f);
        next_free_spr->setOpacity(175);
        next_free_spr->setFlipX(true);

        auto next_free_btn = CCMenuItemSpriteExtra::create(
            next_free_spr,
            this,
            menu_selector(NextFreeLayer::on_next_free)
        );

        next_free_btn->setID("next-free-layer-button");

        layer_btn_mnu->addChild(next_free_btn);
        layer_btn_mnu->setPositionX(new_layer_pos);
        layer_btn_mnu->setLayout(
            RowLayout::create()
                ->setAxisAlignment(AxisAlignment::Center)
        );

        return true;
    }

    void on_next_free(CCObject* sender) {
        auto objs = this->m_editorLayer->m_objects;
        auto next_free = this->m_editorLayer->getNextFreeEditorLayer(objs);
        auto all_layers_btn = this->getChildByIDRecursive("all-layers-button");

        this->m_editorLayer->m_currentLayer = next_free;
        this->m_currentLayerLabel->setString(std::to_string(next_free).c_str());
        all_layers_btn->setVisible(true);
    }

    void onPlaytest(CCObject* sender) {
        auto next_free_button = this->getChildByIDRecursive("next-free-layer-button");
        next_free_button->setVisible(false);

        EditorUI::onPlaytest(sender);
    }
};

struct NextFreeEditorLayer : Modify<NextFreeEditorLayer, LevelEditorLayer> {
    void onStopPlaytest() {
        LevelEditorLayer::onStopPlaytest();

        auto next_free_button = this->m_editorUI->getChildByIDRecursive("next-free-layer-button");
        next_free_button->setVisible(true);
    }
};
