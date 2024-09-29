#pragma once

#include "ToolConfig.hpp"
#include "previewMode/PreviewLogic.hpp"
#include "editMode/EditLogic.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/loader/Mod.hpp>
#ifdef GEODE_IS_WINDOWS
    #include <geode.custom-keybinds/include/Keybinds.hpp>
#endif

#include <sstream>
#include <deque>


using namespace geode::prelude;

class $modify(MyEditorUI, EditorUI) {
    
    struct ModSettings {
        bool m_previewColor = false;
        bool m_ctrlModifierEnabled = false;
        bool m_smartFilter = false;
        bool m_fixedNextFreeItem = true;
        bool m_showOldVariant = false;
        bool m_defaultIsCopyGroup = false;
        bool m_previewModeColorfulLines = false;
        bool m_previewAllWhenNotSelected = true;
        bool m_autoDisable = false;
        bool m_doubleClickModifier = false;
    };

    struct Fields {
        ModSettings m_modSettings;
        CCMenuItemSpriteExtra* m_editButton = nullptr;
        CCMenuItemSpriteExtra* m_previewButton = nullptr;
        CCLabelBMFont* m_debugLabel = nullptr;

        std::deque<unsigned> m_undoTwiceObjUIDs; // for correct work of undo and redo with the tool
        std::deque<unsigned> m_redoTwiceObjUIDs;

        Ref<EditLogic> m_editLogic;
        Ref<PreviewLogic> m_previewLogic;
    };

    // INITS

    void initDebugLabel();
    void initButtons();
    void setKeybinds();
    
    // EDITOR UTILS

    void showDebugText(std::string text, float timeSec=0.5);
    CCPoint screenToEditorLayerPosition(CCPoint screenPoint); // converter
    bool honestAddToGroup(int group, CCArray * objects);

    // BUTTON LISTENERS

    void onMainButtonWrapper(CCObject* sender);
    void onInfoButtonWrapper(CCObject* sender);
    void onInfoButtonWrapper2(CCObject* sender);
    void onPreviewButtonWrapper(CCObject* sender);

    // OVERWRITTEN FUNCTIONS

    $override bool init(LevelEditorLayer * layer);
    $override bool ccTouchBegan(CCTouch * touch, CCEvent * event);
    $override void ccTouchMoved(CCTouch * touch, CCEvent * event);
    $override void ccTouchEnded(CCTouch * touch, CCEvent * event);
    $override void undoLastAction(CCObject * p0);
    $override void redoLastAction(CCObject * p0);
    $override void showUI(bool show);

};
