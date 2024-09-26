#include "PreviewLogic.hpp"
#include "../EditorUI.hpp"

bool PreviewLogic::initPreviewLayer() {
    auto previewLayer = CCDrawNode::create();
    previewLayer->setID("twt-preview-layer");
    auto batchLayer = LevelEditorLayer::get()->m_objectLayer;
    batchLayer->addChild(previewLayer, 1000);
    previewLayer->setZOrder(3000);
    batchLayer->updateChildIndexes();
    m_previewLayer = previewLayer;

    // I have no idea why, but sometimes something 
    // happens and the line drawing is not working (next code tests it)
    // Re-initialization of drawNode usually fixes it
    bool working = m_previewLayer->drawSegment(
        ccp(0, 0), ccp(0, 1), .1f, ccc4f(1, 1, 1, 1));
    static short antiInfiniteRecursion = 10;
    if (!working) {
        log::debug("reinit preview layer");
        if (antiInfiniteRecursion-- < 0) return false;
        m_previewLayer->removeFromParent();
        m_previewLayer = nullptr;
        return initPreviewLayer(); // reinit
    } else {
        antiInfiniteRecursion = 10;
        return true;
    }
}