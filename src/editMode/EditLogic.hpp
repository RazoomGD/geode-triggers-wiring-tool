#pragma once

#include "../ToolConfig.hpp"

#ifdef GEODE_IS_WINDOWS
    #include <geode.custom-keybinds/include/Keybinds.hpp>
#endif

using namespace geode::prelude;

struct MyEditorUI;

class EditLogic : public CCNode {
private:
    struct ToolConfig {
        Variant m_variant; // is set in upper menu
        short m_group; // is set in lower menu
        bool m_isFinished = false; // if we've created both upper and lower menus
        short m_nextFreeGroup;
    };

    MyEditorUI * m_editorInstance = nullptr;
    CCDrawNode * m_drawingLayer = nullptr;

    CCPoint m_lineStart;  // both are editor coordinates
    CCPoint m_lineEnd;
    Ref<CCArray> m_objectsSource = nullptr;  // array of selected objects
    std::pair<CCPoint, CCPoint> m_sourceRectangle;
    Ref<GameObject> m_objectTarget = nullptr;  // end object

    Ref<CCMenu> m_upperMenu = nullptr;
    Ref<CCMenu> m_lowerMenu = nullptr;

    Ref<CCArray> m_objectsSourceCopy = nullptr;
    Ref<CCArray> m_objectsSourceFiltered = nullptr;

    std::string m_objectTargetInitial;
    std::string m_objectTargetLastUse;

    bool m_buttonIsActivated = false;
    bool m_interfaceIsVisible = false;
    bool m_panEditor = false;

    ToolConfig m_globalConfig;

    EditLogic();
    ~EditLogic();

    void setKeybinds();
    void initDrawingLayer();
    void updateLineAndRect(bool clear);
    void controlTargetObjectCallback(float);

    bool handleTargetObject();
    void applyToolConfig(bool updateSourceObjects);
    CCPoint calculateLineStartOnRectangle(CCPoint endPoint, std::pair<CCPoint, CCPoint> box);

    void createUpperMenu(const std::vector<Variant>& configuration, bool selectFirst, std::set<srcObjType> commonType);
    void upperMenuActionListener(CCObject * sender);

    void createLowerMenuForVariant(Variant variant);
    void createLowerMenu(const std::vector<std::pair<std::string, int>>& configuration, bool selectFirst);
    void lowerMenuActionListener(CCObject * sender);

    bool toolIsActivated();
    void resetTool();

public:
    bool init();
    static EditLogic * create(MyEditorUI * editor);

    void onMainButton(CCObject * sender);
    void onInfoButton(CCObject * sender);

    bool handleTouchStart(const CCTouch * const touch);
    bool handleTouchMiddle(const CCTouch * const touch);
    bool handleTouchEnd(bool select, bool showDebug);

};
