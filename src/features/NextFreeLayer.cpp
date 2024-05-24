#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

using namespace geode::prelude;

struct NextFreeLayer : Modify<NextFreeLayer, EditorUI> {
    bool init(LevelEditorLayer *editorLayer) {
        if (!EditorUI::init(editorLayer)) {
            return false;
        }

        auto layer_btn_mnu = this->getChildByIDRecursive("layer-menu");
        auto next_free_spr = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
        auto new_layr_pos = layer_btn_mnu->getPositionX() - 10.f;
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
        layer_btn_mnu->setPositionX(new_layr_pos);
        layer_btn_mnu->setLayout(
                RowLayout::create()
                        ->setAxisAlignment(AxisAlignment::Center)
        );

        return true;
    }

    void on_next_free(CCObject *sender) {
        auto objs = this->m_editorLayer->m_objects;
        auto all_layers_btn = this->getChildByIDRecursive("all-layers-button");

        std::vector<int> layers;

        for (GameObject *obj: CCArrayExt<GameObject *>(objs)) {
            if (std::find(layers.begin(), layers.end(), obj->m_editorLayer) == layers.end()) {
                layers.push_back(obj->m_editorLayer);
            }

            if (std::find(layers.begin(), layers.end(), obj->m_editorLayer2) == layers.end()) {
                layers.push_back(obj->m_editorLayer2);
            }
        }

        std::sort(layers.begin(), layers.end());

        int last = -1;

        for (int layer: layers) {
            if (last + 1 != layer) {
                break;
            }

            last = layer;
        }

        this->m_editorLayer->m_currentLayer = last + 1;
        this->m_currentLayerLabel->setString(
                CCString::createWithFormat("%d", last + 1)->getCString()
        );

        all_layers_btn->setVisible(true);
    }

    void onPlaytest(CCObject *sender) {
        auto next_free_button = this->getChildByIDRecursive("next-free-layer-button");
        next_free_button->setVisible(false);

        EditorUI::onPlaytest(sender);
    }
};

struct NextFreeEditorLayer : Modify<NextFreeEditorLayer, LevelEditorLayer> {
    bool init(GJGameLevel *p0, bool p1) {
        if (!LevelEditorLayer::init(p0, p1)) {
            return false;
        }

        return true;
    }

    void onStopPlaytest() {
        LevelEditorLayer::onStopPlaytest();

        auto next_free_button = this->m_editorUI->getChildByIDRecursive("next-free-layer-button");
        next_free_button->setVisible(true);
    }
};
