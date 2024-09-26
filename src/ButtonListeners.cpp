#include "EditorUI.hpp"

// edit mode
void MyEditorUI::onMainButtonWrapper(CCObject* sender) {
    m_fields->m_editLogic->onMainButton(sender);
}

void MyEditorUI::onInfoButtonWrapper(CCObject* sender) {
    m_fields->m_editLogic->onInfoButton(sender);
}

// preview mode
void MyEditorUI::onPreviewButtonWrapper(CCObject* sender) {
    m_fields->m_previewLogic->togglePreviewMode(sender);
}

void MyEditorUI::onInfoButtonWrapper2(CCObject* sender) {
    m_fields->m_previewLogic->onInfoButton(sender);
}
