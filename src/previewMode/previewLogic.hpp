#pragma once

#include "PreviewConfig.hpp"

#ifdef GEODE_IS_WINDOWS
    #include <geode.custom-keybinds/include/Keybinds.hpp>
#endif

using namespace geode::prelude;

struct MyEditorUI;

class PreviewLogic : public CCNode {
private:
    MyEditorUI * m_editorInstance = nullptr;
    CCDrawNode * m_previewLayer = nullptr;

    bool m_previewIsOn = false;


    PreviewLogic();
    ~PreviewLogic();

    bool initPreviewLayer();
    // void setKeybinds();

    void drawPreview(GameObject * const center, CCArray * const sourceObjects);

    void drawLineAndRectangle(const CCPoint &startLine, const CCPoint &topLeftRect, 
        const CCPoint &bottomRightRect, std::optional<int> randomColorSeed);

    // void resetPreviewMode();
    void updatePreview();

    void findObjectsOfType(CCArray * const objects, std::vector<SearchResult> &categories);


public: 
    bool init();
    static PreviewLogic * create(MyEditorUI * editor);

    void togglePreviewMode(CCObject * sender);
    void onInfoButton(CCObject * sender);

};
