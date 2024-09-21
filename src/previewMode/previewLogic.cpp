#include "previewLogic.hpp"

using namespace geode::prelude;

PreviewLogic::PreviewLogic() {
    this->autorelease();
}

bool PreviewLogic::init() {
    log::debug("init preview");
    return true;
}

PreviewLogic * PreviewLogic::create() {
    auto ret = new (std::nothrow) PreviewLogic();
    if (ret && ret->init()) {
        return ret;
    }
    delete ret;
    return nullptr;
}