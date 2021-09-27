#include "../ContextMenu.hpp"
#include "../CustomizeCMLayer.hpp"
#include "../../EditorLayerInput/editorLayerInput.hpp"

#define MORD(x, d) (this->m_pMenu ? this->m_pMenu->x : d)

ActionContextMenuItem::ACMIAction ActionContextMenuItem::actionForID(std::string const& id) {
    switch (hash(id.c_str())) {
        case "deselect_this"_h: return {
            "Deselect This",
            [](ActionContextMenuItem* item, GameObject* obj) -> void {
                auto ui = LevelEditorLayer::get()->getEditorUI();
                CATCH_NULL(ui->m_pSelectedObjects)->removeObject(obj);
                if (ui->m_pSelectedObject) {
                    ui->m_pSelectedObject = nullptr;
                }
                if (item->m_pMenu) {
                    item->m_pMenu->m_pObjSelectedUnderMouse = nullptr;
                }
                obj->deselectObject();
                ui->updateButtons();
                ui->updateObjectInfoLabel();
            }
        };
        case "select_structure"_h: return {
            "Select Structure",
            [](ActionContextMenuItem* item, GameObject* obj) -> void {
                auto ui = LevelEditorLayer::get()->getEditorUI();
                selectStructure(ui, obj);
                ui->updateButtons();
                ui->updateObjectInfoLabel();
                if (item->m_pMenu) {
                    item->m_pMenu->m_bDeselectObjUnderMouse = false;
                }
            }
        };
        default:
            return { "Unknown Action" };
    }
}

bool ActionContextMenuItem::init(ContextMenu* menu, std::string const& id) {
    if (!ContextMenuItem::init(menu))
        return false;
    
    this->m_obAction = this->actionForID(id);

    this->m_pLabel = this->createLabel(this->m_obAction.m_sName.c_str());
    this->m_pLabel->setColor(to3B(m_pMenu->m_colText));
    this->addChild(this->m_pLabel);

    return true;
}

void ActionContextMenuItem::visit() {
    this->m_pLabel->setPosition(
        this->getContentSize() / 2
    );
    this->m_pLabel->limitLabelWidth(
        this->getScaledContentSize().width - 10.0f,
        this->m_pMenu ? this->m_pMenu->m_fFontScale : .4f,
        .02f
    );

    ContextMenuItem::visit();
}

void ActionContextMenuItem::activate() {
    if (this->m_pMenu) {
        this->m_obAction.m_callback(this, this->m_pMenu->m_pObjSelectedUnderMouse);
        this->m_pMenu->hide();
    } else {
        this->m_obAction.m_callback(this, nullptr);
    }
    SuperMouseManager::get()->releaseCapture(this);
}

ActionContextMenuItem* ActionContextMenuItem::create(
    ContextMenu* menu, std::string const& id
) {
    auto ret = new ActionContextMenuItem;

    if (ret && ret->init(menu, id)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
