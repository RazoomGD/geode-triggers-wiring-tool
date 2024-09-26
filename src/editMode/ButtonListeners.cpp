#include "EditLogic.hpp"
#include "../EditorUI.hpp"

void EditLogic::onMainButton(CCObject * sender) {
    auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
    auto oldSprite = btn->getNormalImage();
    m_buttonIsActivated = !m_buttonIsActivated;
    if (m_buttonIsActivated) {
        auto sprite = CCSprite::createWithSpriteFrameName("TWT_tool_on.png"_spr);
        sprite->setScale(oldSprite->getScale());
        btn->setSprite(sprite);
        m_editorInstance->showDebugText("Edit mode is on");
    } else {
        auto sprite = CCSprite::createWithSpriteFrameName("TWT_tool_off.png"_spr);
        sprite->setScale(oldSprite->getScale());
        btn->setSprite(sprite);
        // m_fields->m_panEditor = false;
        resetTool();
        m_editorInstance->showDebugText("Edit mode is off");
    }
}

void EditLogic::onInfoButton(CCObject *) { // todo: change description
    FLAlertLayer::create(
        "Triggers Wiring Tool",
        "todo : description",
        "OK"
    )->show();
}
