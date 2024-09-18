#include "EditorUI.hpp"
#include "ToolUtils.hpp"

void MyEditorUI::onMainButton(CCObject* sender) {
    auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
    auto oldSprite = btn->getNormalImage();
    m_fields->m_buttonIsActivated = !m_fields->m_buttonIsActivated;
    if (m_fields->m_buttonIsActivated) {
        auto sprite = CCSprite::createWithSpriteFrameName("TWT_tool_on.png"_spr);
        sprite->setScale(oldSprite->getScale());
        btn->setSprite(sprite);
        showDebugText("Edit mode is on");
    } else {
        auto sprite = CCSprite::createWithSpriteFrameName("TWT_tool_off.png"_spr);
        sprite->setScale(oldSprite->getScale());
        btn->setSprite(sprite);
        m_fields->m_panEditor = false;
        resetTool();
        showDebugText("Edit mode is off");
    }
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
        m_fields->m_panEditor = false;
        resetTool();
        showDebugText("Preview mode is off");
    }
}


void MyEditorUI::onInfoButton(CCObject*) { // todo: change description

    FLAlertLayer::create(
        "Triggers Wiring Tool",
        "todo : description",
        "OK"
    )->show();
}

void MyEditorUI::onModeChanged(CCObject* sender) {
    Mode newMode = static_cast<Mode>(sender->getTag());
    if (m_fields->m_mode == newMode) return;
    m_fields->m_mode = newMode;
    resetTool();
    if (newMode == Mode::SetupMode) {
        showDebugText("Setup mode");
    } else {
        showDebugText("Preview mode");

    }
}