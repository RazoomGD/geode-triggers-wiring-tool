#include "EditorUI.hpp"

bool MyEditorUI::init(LevelEditorLayer * layer) {
    if (!EditorUI::init(layer)) return false;
    // init mod settings
    m_fields->m_modSettings.m_previewColor =  Mod::get()->template  getSettingValue<bool>("preview-color");
    m_fields->m_modSettings.m_smartFilter =  Mod::get()->template getSettingValue<bool>("smart-filter");
    m_fields->m_modSettings.m_fixedNextFreeItem =  Mod::get()->template getSettingValue<bool>("fix-next-item-id");
    #ifdef GEODE_IS_WINDOWS
        m_fields->m_modSettings.m_ctrlModifierEnabled = Mod::get()->template getSettingValue<bool>("control-key-modifier");
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

void MyEditorUI::undoLastAction(CCObject * p0) {
    auto levelLayer = LevelEditorLayer::get();
    if (!levelLayer->m_undoObjects->count()) return;
    auto lastUndo = static_cast<UndoObject*>(levelLayer->m_undoObjects->lastObject());
    auto& undo = m_fields->m_undoTwiceObjUIDs;
    EditorUI::undoLastAction(p0);
    if (levelLayer->m_undoObjects->count() && 
        std::find(undo.begin(), undo.end(), lastUndo->m_uID) != undo.end()) {
        MyEditorUI::undoLastAction(p0);
        log::debug("undone 2 times");
    }
}

void MyEditorUI::redoLastAction(CCObject * p0) {
    auto levelLayer = LevelEditorLayer::get();
    if (!levelLayer->m_redoObjects->count()) return;
    auto lastRedo = static_cast<UndoObject*>(levelLayer->m_redoObjects->lastObject());
    auto& redo = m_fields->m_redoTwiceObjUIDs;
    EditorUI::redoLastAction(p0);
    if (levelLayer->m_redoObjects->count() && 
        std::find(redo.begin(), redo.end(), lastRedo->m_uID) != redo.end()) {
        MyEditorUI::redoLastAction(p0);
        log::debug("redone 2 times");
    }
}
