#include "PreviewLogic.hpp"

using namespace geode::prelude;

PreviewLogic::PreviewLogic() {
    this->autorelease();
}

PreviewLogic::~PreviewLogic() {}

bool PreviewLogic::init() {
    return true;
}

PreviewLogic * PreviewLogic::create(MyEditorUI * editor) {
    auto ret = new (std::nothrow) PreviewLogic();
    if (ret) {
        ret->m_editorInstance = editor;
        if (ret->init()) return ret;
    }
    delete ret;
    return nullptr;
}
