#include "EditorUI.hpp"

void MyEditorUI::showDebugText(std::string text, float timeSec) {
    if (!m_fields->m_debugLabel) initDebugLabel();
    auto label = m_fields->m_debugLabel;
    label->setString(text.c_str());
    label->setOpacity(255);
    label->stopAllActions();
    label->runAction(CCSequence::create(
        CCDelayTime::create(timeSec), CCFadeOut::create(timeSec), nullptr));
}

CCPoint MyEditorUI::screenToEditorLayerPosition(CCPoint screenPoint) {
    auto glPoint = CCDirector::get()->convertToGL(screenPoint);
    auto objectLayer = LevelEditorLayer::get()->m_objectLayer;
    return objectLayer->convertToNodeSpace(this->convertToWorldSpace(glPoint));
}