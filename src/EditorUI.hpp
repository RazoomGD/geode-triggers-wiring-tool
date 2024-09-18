#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/loader/Mod.hpp>

#include <utility>
#include <sstream>
#include <optional>
#include <cstring>
#include <deque>

#ifdef GEODE_IS_WINDOWS
#include <geode.custom-keybinds/include/Keybinds.hpp>
#endif

using namespace geode::prelude;

#include "ToolConfig.hpp"

// I was a bit lazy to divide this into several header files (sorry)

class $modify(MyEditorUI, EditorUI) {
    struct ToolConfig {
        Variant m_variant; // is set in upper menu
        short m_group; // is set in lower menu
        bool m_isFinished = false; // if we've created both upper and lower menus
        short m_nextFreeGroup;
    };

    struct ModSettings {
        bool m_previewColor = false;
        bool m_ctrlModifierEnabled = false;
        bool m_smartFilter = false;
        bool m_fixedNextFreeItem = true;
        bool m_showOldVariant = false;
    };

    enum Mode {
        SetupMode, PreviewMode
    };

    struct Fields {
        Mode m_mode = Mode::SetupMode;
        ModSettings m_modSettings;
        CCMenuItemSpriteExtra* m_button = nullptr;
        CCMenuItemSpriteExtra* m_button2 = nullptr;
        CCLabelBMFont* m_debugLabel = nullptr;
        // for setup mode
        CCDrawNode* m_drawingLayer = nullptr;
        CCPoint m_lineStart;  // both are editor coordinates
        CCPoint m_lineEnd;
        std::pair<CCPoint, CCPoint> m_sourceRectangle;
        Ref<CCArray> m_objectsSource = nullptr;  // array of selected objects
        Ref<GameObject> m_objectTarget = nullptr;  // end object
        Ref<CCArray> m_objectsSourceCopy = nullptr;
        Ref<CCArray> m_objectsSourceFiltered = nullptr;
        std::string m_objectTargetInitial;
        std::string m_objectTargetLastUse;
        Ref<CCMenu> m_upperMenu = nullptr;
        Ref<CCMenu> m_lowerMenu = nullptr;
        bool m_buttonIsActivated = false;
        bool m_interfaceIsVisible = false;
        bool m_panEditor = false;
        ToolConfig m_globalConfig;
        std::deque<unsigned> m_undoTwiceObjUIDs; // for correct work of undo and redo with the tool
        std::deque<unsigned> m_redoTwiceObjUIDs;
        // for preview mode
    };

    // INITS

    void initDrawingLayer();
    void initDebugLabel();
    void initButtons();
    void setKeybinds();
    
    // EDITOR UTILS

    void showDebugText(std::string text, float timeSec=0.5);
    CCPoint screenToEditorLayerPosition(CCPoint screenPoint); // converter
    bool honestAddToGroup(int group, CCArray * objects);

    // BUTTON LISTENERS

    void onMainButton(CCObject* sender);
    void onPreviewButton(CCObject* sender);
    void onInfoButton(CCObject*);
    void onModeChanged(CCObject* sender);

    // OVERWRITTEN FUNCTIONS

    $override bool init(LevelEditorLayer * layer);
    $override bool ccTouchBegan(CCTouch * touch, CCEvent * event);
    $override void ccTouchMoved(CCTouch * touch, CCEvent * event);
    $override void ccTouchEnded(CCTouch * touch, CCEvent * event);
    $override void undoLastAction(CCObject * p0);
    $override void redoLastAction(CCObject * p0);
    $override void showUI(bool show);

    // TOOL TOUCH LOGIC

    bool handleTouchStart(const CCTouch * const touch);
    void handleTouchMiddle(const CCTouch * const touch);
    void handleTouchEnd(bool select, bool showDebug);

    // MENUS

    void createUpperMenu(const std::vector<Variant>& configuration, bool selectFirst, std::set<srcObjType> commonType);
    void upperMenuActionListener(CCObject * sender);

    void createLowerMenuForVariant(Variant variant);
    void createLowerMenu(const std::vector<std::pair<std::string, int>>& configuration, bool selectFirst);
    void lowerMenuActionListener(CCObject * sender);

    // TOOL MAIN LOGIC

    bool toolIsActivated();
    void applyToolConfig(bool updateSourceObjects=true);
    bool handleTargetObject();

    // TOOL UTILS

    CCPoint calculateLineStartOnRectangle(CCPoint endPoint, std::pair<CCPoint, CCPoint> box);


    // TOOL INTERFACE
    
    void updateLineAndRect(bool clear);
    void controlTargetObjectCallback(float);
    void resetTool();

};
