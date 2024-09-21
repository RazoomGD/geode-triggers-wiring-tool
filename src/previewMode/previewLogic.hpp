#pragma once

#include "../ToolConfig.hpp"

class PreviewLogic : public cocos2d::CCNode {
private:
    cocos2d::CCDrawNode * m_previewLayer;

    PreviewLogic();

public: 
    bool init();
    static PreviewLogic * create();

};
