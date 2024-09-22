#include "EditorUI.hpp"

void MyEditorUI::onMainButtonWrapper(CCObject* sender) {
    m_fields->m_editLogic->onMainButton(sender);
}

void MyEditorUI::onPreviewButton(CCObject* sender) {
    auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
    auto oldSprite = btn->getNormalImage();
    // m_fields->m_buttonIsActivated = !m_fields->m_buttonIsActivated;
    if (false /*todo:*/) {
        auto sprite = CCSprite::createWithSpriteFrameName("TWT_preview_tool_on.png"_spr);
        sprite->setScale(oldSprite->getScale());
        btn->setSprite(sprite);
        showDebugText("Preview mode is on");
    } else {
        auto sprite = CCSprite::createWithSpriteFrameName("TWT_preview_tool_off.png"_spr);
        sprite->setScale(oldSprite->getScale());
        btn->setSprite(sprite);
        // m_fields->m_panEditor = false;
        // resetTool();
        showDebugText("Preview mode is off");
    }
}
