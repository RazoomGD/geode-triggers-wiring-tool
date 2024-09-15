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

bool MyEditorUI::honestAddToGroup(int group, CCArray * objects) {
    EditorUI::editGroup(nullptr);
    auto setGrLayer = static_cast<SetGroupIDLayer*>(
        CCScene::get()->getChildByID("SetGroupIDLayer"));
    if (!setGrLayer) {
        return false;
    }
    setGrLayer->m_groupIDValue = group;
    setGrLayer->m_hasTargetObjects = false;
    setGrLayer->m_targetObjects = nullptr;
    for (unsigned i = 0; i < objects->count(); i++) {
        setGrLayer->m_targetObject = static_cast<GameObject*>(objects->objectAtIndex(i));
        setGrLayer->onAddGroup(nullptr);
    }
    setGrLayer->onClose(nullptr);
    return true;
}