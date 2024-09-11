#include "EditorUI.hpp"

void MyEditorUI::onMainButton(CCObject* sender) {
    auto oldSprite = static_cast<CCMenuItemSpriteExtra*>(sender)->getNormalImage();
    m_fields->m_buttonIsActivated = !m_fields->m_buttonIsActivated;
    auto btn = m_fields->m_button;
    if (m_fields->m_buttonIsActivated) {
        auto sprite = CCSprite::createWithSpriteFrameName("TWT_tool_on.png"_spr);
        sprite->setScale(oldSprite->getScale());
        btn->setSprite(sprite);
        showDebugText("Tool is on");
    } else {
        auto sprite = CCSprite::createWithSpriteFrameName("TWT_tool_off.png"_spr);
        sprite->setScale(oldSprite->getScale());
        btn->setSprite(sprite);
        m_fields->m_panEditor = false;
        resetTool();
        showDebugText("Tool is off");
    }
}

void MyEditorUI::onInfoButton(CCObject*) { // todo: change description
    int id = getNextFreeItemFixed();    
    FLAlertLayer::create(
        "Color Example",
        std::format("next id = {}", id),
        "OK"
    )->show();
    // auto obj = static_cast<LabelGameObject*>(EditorUI::getSelectedObjects()->objectAtIndex(0));
    // auto num = LevelEditorLayer::get()->getNextFreeItemID(nullptr);
    // getItemsAllIds(EditorUI::getSelectedObjects());
    // log::debug("item id = {}", obj->m_itemID);
    // log::debug("is pickup = {}", obj->m_collectibleIsPickupItem);
    // log::debug("item id 2 = {}", obj->m_itemID2);

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