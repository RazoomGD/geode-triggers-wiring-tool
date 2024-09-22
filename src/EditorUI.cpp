#include "EditorUI.hpp"

bool MyEditorUI::init(LevelEditorLayer * layer) {
    if (!EditorUI::init(layer)) return false;
    // init mod settings
    m_fields->m_modSettings.m_previewColor =  Mod::get()->template  getSettingValue<bool>("preview-color");
    m_fields->m_modSettings.m_smartFilter =  Mod::get()->template getSettingValue<bool>("smart-filter");
    m_fields->m_modSettings.m_fixedNextFreeItem =  Mod::get()->template getSettingValue<bool>("fix-next-item-id");
    m_fields->m_modSettings.m_showOldVariant =  Mod::get()->template getSettingValue<bool>("show-old-variant");
    #ifdef GEODE_IS_WINDOWS
        m_fields->m_modSettings.m_ctrlModifierEnabled = Mod::get()->template getSettingValue<bool>("control-key-modifier");
    #endif
    // init preview mode logic
    m_fields->m_previewLogic = PreviewLogic::create();
    if (!m_fields->m_previewLogic) return false;

    // init edit mode logic
    m_fields->m_editLogic = EditLogic::create(this);
    if (!m_fields->m_editLogic) return false;
    m_fields->m_editLogic->setID("twt-edit-logic");
    this->addChild(m_fields->m_editLogic);

    initButtons();
    setKeybinds();

    return true;
}

bool MyEditorUI::ccTouchBegan(CCTouch * touch, CCEvent * event) {
    if (m_fields->m_editLogic->handleTouchStart(touch)) {
        return true;
    } else {
        return EditorUI::ccTouchBegan(touch, event);
    }
}

void MyEditorUI::ccTouchMoved(CCTouch * touch, CCEvent * event) {
    if (!m_fields->m_editLogic->handleTouchMiddle(touch)) {
        EditorUI::ccTouchMoved(touch, event);
    }
}

void MyEditorUI::ccTouchEnded(CCTouch * touch, CCEvent * event) {
    if (!m_fields->m_editLogic->handleTouchEnd(touch, true)) {
        EditorUI::ccTouchEnded(touch, event);
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
    }
}

void MyEditorUI::showUI(bool show) {
    EditorUI::showUI(show);
    if (auto undoMenu = this->getChildByID("undo-menu")) {
        if (auto myMenu = static_cast<CCNode*>(undoMenu)->getChildByID("twt-menu")) {
            myMenu->setVisible(show);
        }
    }
}
