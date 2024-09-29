#include "EditLogic.hpp"
#include "../EditorUI.hpp"

void EditLogic::initDrawingLayer() {
    m_drawingLayer = CCDrawNode::create();
    m_drawingLayer->setID("twt-drawing-layer");
    auto batchLayer = LevelEditorLayer::get()->m_objectLayer;
    batchLayer->addChild(m_drawingLayer, 3000);

    // I have no idea why, but sometimes something 
    // happens and the line drawing is not working (next code tests it)
    // Re-initialization of drawNode usually fixes it
    bool working = m_drawingLayer->drawSegment(
        ccp(0, 0), ccp(0, 1), .1f, ccc4f(1, 1, 1, 1));
    static short antiInfiniteRecursion = 10;
    if (!working) {
        if (antiInfiniteRecursion-- < 0) return;
        m_drawingLayer->removeFromParent();
        m_drawingLayer = nullptr;
        initDrawingLayer(); // reinit
    } else {
        antiInfiniteRecursion = 10;
    }
}

void EditLogic::setKeybinds() {
    #ifdef GEODE_IS_WINDOWS 
    this->template addEventListener<keybinds::InvokeBindFilter>(
        [=](keybinds::InvokeBindEvent* event) {
            if (event->isDown()) {
                m_panEditor = true;
            } else {
                m_panEditor = false;
            }
            return ListenerResult::Propagate;
        },
        "twt-pan-in-the-editor"_spr);
    #endif
}
