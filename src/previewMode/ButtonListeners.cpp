#include "PreviewLogic.hpp"
#include "../EditorUI.hpp"

void PreviewLogic::onPreviewButton(CCObject* sender) {
    auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
    auto oldSprite = btn->getNormalImage();
    m_previewIsOn = !m_previewIsOn;
    if (m_previewIsOn) {
        auto sprite = CCSprite::createWithSpriteFrameName("TWT_preview_tool_on.png"_spr);
        sprite->setScale(oldSprite->getScale());
        btn->setSprite(sprite);
        m_editorInstance->showDebugText("Preview mode is on");
    } else {
        auto sprite = CCSprite::createWithSpriteFrameName("TWT_preview_tool_off.png"_spr);
        sprite->setScale(oldSprite->getScale());
        btn->setSprite(sprite);
        // resetPreviewMode(); todo:
        m_editorInstance->showDebugText("Preview mode is off");
    }
}

void PreviewLogic::onInfoButton(CCObject *) { // todo: change description

    FLAlertLayer::create(
        "Triggers Wiring Tool",
        "todo : description 2",
        "OK"
    )->show();
}

