#include "EditLogic.hpp"

EditLogic::EditLogic() {
    this->autorelease();
    log::debug("construct edit logic done");
}

EditLogic::~EditLogic() {
    log::debug("delete edit logic done");
}
 
bool EditLogic::init() {
    // initDrawingLayer();
    setKeybinds();
    this->schedule(SEL_SCHEDULE(&EditLogic::controlTargetObjectCallback), 0.f);
    initDrawingLayer();
    log::debug("init edit logic done");
    return true;
}

EditLogic * EditLogic::create(MyEditorUI * editor) {
    auto ret = new (std::nothrow) EditLogic();
    if (ret) {
        ret->m_editorInstance = editor;
        if (ret->init()) {
            log::debug("create edit logic done");
            return ret;
        }
    }
    log::debug("error on create edit logic");
    delete ret;
    return nullptr;
}


