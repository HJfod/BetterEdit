#include "InputScaleDelegate.hpp"

void InputScaleDelegate::textChanged(CCTextInputNode* input) {
    float val = 1.0f;

    if (input->getString() && strlen(input->getString())) {
        val = static_cast<float>(std::atof(input->getString()));
    }

    auto ui = GameManager::sharedState()
        ->getEditorLayer()
        ->m_editorUI;
    
    if (ui) {
        ui->scaleChanged(val);
    }
}