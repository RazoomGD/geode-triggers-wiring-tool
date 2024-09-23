#pragma once

#include "../ToolConfig.hpp"

#ifdef GEODE_IS_WINDOWS
    #include <geode.custom-keybinds/include/Keybinds.hpp>
#endif

using namespace geode::prelude;

struct MyEditorUI;

class PreviewLogic : public CCNode {
private:
    MyEditorUI * m_editorInstance = nullptr;
    CCDrawNode * m_drawingLayer = nullptr;

    bool m_previewIsOn = false;


    PreviewLogic();
    ~PreviewLogic();

    // void initPreviewLayer();
    // void setKeybinds();

    // void showPreview(CCObject * center, CCArray * sourceObjects);

    // void drawLineAndRectangle(CCPoint * startLine, CCPoint * topLeftRect, CCPoint * bottomRightRect);

    // void resetPreviewMode();


public: 
    bool init();
    static PreviewLogic * create(MyEditorUI * editor);

    void onPreviewButton(CCObject * sender);
    void onInfoButton(CCObject * sender);

};
