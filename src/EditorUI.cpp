#include "EditorUI.hpp"

bool MyEditorUI::init(LevelEditorLayer * layer) {
    if (!EditorUI::init(layer)) return false;
    // init mod settings
    m_fields->m_modSettings.m_previewColor =  Mod::get()->getSettingValue<bool>("preview-color");
    m_fields->m_modSettings.m_smartFilter =  Mod::get()->getSettingValue<bool>("smart-filter");
    m_fields->m_modSettings.m_fixedNextFreeItem =  Mod::get()->getSettingValue<bool>("fix-next-item-id");
    #ifdef GEODE_IS_WINDOWS
        m_fields->m_modSettings.m_ctrlModifierEnabled = Mod::get()->getSettingValue<bool>("control-key-modifier");
    #endif
    initButtons();
    setKeybinds();
    this->schedule(SEL_SCHEDULE(&MyEditorUI::controlTargetObjectCallback), 0.f);
    return true;
}

bool MyEditorUI::ccTouchBegan(CCTouch * touch, CCEvent * event) {
    if (toolIsActivated()) {
        if (m_fields->m_interfaceIsVisible) {
            resetTool();
        }
        auto startedUsingTool = handleTouchStart(touch);
        if (startedUsingTool) {
            m_fields->m_interfaceIsVisible = true;
            return true;
        } else {
            return EditorUI::ccTouchBegan(touch, event);
        }
    } else {
        return EditorUI::ccTouchBegan(touch, event);
    }
}

void MyEditorUI::ccTouchMoved(CCTouch * touch, CCEvent * event) {
    if (toolIsActivated() && m_fields->m_interfaceIsVisible) {
        handleTouchMiddle(touch);
    } else {
        EditorUI::ccTouchMoved(touch, event);
        if (m_fields->m_interfaceIsVisible && !m_fields->m_objectTarget) {
            resetTool();
        }
    }
}

void MyEditorUI::ccTouchEnded(CCTouch * touch, CCEvent * event) {
    if (toolIsActivated() && m_fields->m_interfaceIsVisible) {
        handleTouchEnd(touch, true);
    } else {
        EditorUI::ccTouchEnded(touch, event);
        if (m_fields->m_interfaceIsVisible && !m_fields->m_objectTarget) {
            resetTool();
        }
    }
}