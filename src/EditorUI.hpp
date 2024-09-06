#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

#include <utility>
#include <sstream>
#include <optional>
#include <cstring>

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
    };

    struct ModSettings {
        bool m_previewColor = false;
        bool m_ctrlModifierEnabled = false;
        bool m_smartFilter = false;
    };

    enum Mode {
        SetupMode, PreviewMode
    };

    struct Fields {
        Mode m_mode = Mode::SetupMode;
        ModSettings m_modSettings;
        CCMenuItemSpriteExtra* m_button = nullptr;
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

    // BUTTON LISTENERS

    void onMainButton(CCObject* sender);
    void onInfoButton(CCObject*);
    void onModeChanged(CCObject* sender);

    // OVERWRITTEN FUNCTIONS

    $override bool init(LevelEditorLayer * layer);
    $override bool ccTouchBegan(CCTouch * touch, CCEvent * event);
    $override void ccTouchMoved(CCTouch * touch, CCEvent * event);
    $override void ccTouchEnded(CCTouch * touch, CCEvent * event);

    // TOOL TOUCH LOGIC

    bool handleTouchStart(const CCTouch * const touch);
    void handleTouchMiddle(const CCTouch * const touch);
    void handleTouchEnd(bool select, bool showDebug);

    // MENUS

    void createUpperMenu(const std::vector<Variant>& configuration, bool selectFirst, srcObjType commonType);
    void upperMenuActionListener(CCObject * sender);

    void createLowerMenuForVariant(Variant variant);
    void createLowerMenu(const std::vector<std::pair<std::string, int>>& configuration, bool selectFirst);
    void lowerMenuActionListener(CCObject * sender);

    // TOOL MAIN LOGIC

    bool toolIsActivated();
    void applyToolConfig();
    bool handleTargetObject();

    // TOOL UTILS

    srcObjType getTypeById(short objId);
    std::set<srcObjType> getObjectsAllTypes(CCArray * objects);
    CCArray * filterObjectsByType(srcObjType objType, CCArray * objects, bool color);

    std::vector<short> getObjectsCommonGroups(CCArray * objects);
    bool isNewGroupPossible(CCArray * objects);
    void addToGroup(int group, CCArray * objects);
    void addToGroupSM(int group, CCArray * objects);
    void addToGroupAnim(int group, CCArray * objects);
    void myCopyObjectProps(GameObject * from, TWTObjCopy * to);
    void myPasteObjectProps(TWTObjCopy * from, GameObject * to);

    std::vector<int> getObjectsAllColors(CCArray * objects);
    std::optional<int> getCommonBaseColor(CCArray * objects);
    std::optional<int> getCommonDetailColor(CCArray * objects);

    std::map<std::string, std::string> objectToKeyVal(std::string objSaveString);
    std::string applyDifference(std::string before, std::string after, std::string obj);
    CCPoint calculateLineStartOnRectangle(CCPoint endPoint, std::pair<CCPoint, CCPoint> box);


    // TOOL INTERFACE
    
    void updateLineAndRect(bool clear);
    void controlTargetObjectCallback(float);
    void resetTool();

};
