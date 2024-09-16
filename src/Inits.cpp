#include "EditorUI.hpp"

void MyEditorUI::setKeybinds() {
    #ifdef GEODE_IS_WINDOWS 
    this->template addEventListener<keybinds::InvokeBindFilter>(
        [=](keybinds::InvokeBindEvent* event) {
            if (event->isDown()) {
                m_fields->m_panEditor = true;
            } else {
                m_fields->m_panEditor = false;
            }
            return ListenerResult::Propagate;
        },
        "twt-pan-in-the-editor"_spr);

    this->template addEventListener<keybinds::InvokeBindFilter>(
        [=](keybinds::InvokeBindEvent* event) {
            if (event->isDown()) {
                this->onMainButton(m_fields->m_button);
            }
            return ListenerResult::Propagate;
        },
        "twt-activate-tool"_spr);
    #endif
}

void MyEditorUI::initDrawingLayer() {
    auto drawingLayer = CCDrawNode::create();
    drawingLayer->setID("twt-drawing-layer");
    auto parent = this->getParent()->getChildByID("main-node")->getChildByID("batch-layer");
    parent->addChild(drawingLayer, 1000);
    drawingLayer->setZOrder(3000);
    parent->updateChildIndexes();
    m_fields->m_drawingLayer = drawingLayer;

    // I have no idea why, but sometimes something 
    // happens and the line drawing is not working (next code tests it)
    // Re-initializing drawing layer usually fixes it
    bool working = m_fields->m_drawingLayer->drawSegment(
        ccp(0, 0), ccp(0, 1), .1f, ccc4f(1, 1, 1, 1));
    static short antiInfiniteRecursion = 10;
    if (!working) {
        log::debug("reinit drawing layer");
        if (antiInfiniteRecursion-- < 0) return;
        m_fields->m_drawingLayer->removeFromParent();
        m_fields->m_drawingLayer = nullptr;
        initDrawingLayer(); // reinit
    } else {
        antiInfiniteRecursion = 10;
    }
}

void MyEditorUI::initDebugLabel() {
    auto debugLabel = CCLabelBMFont::create("", "bigFont.fnt");
    auto btn = m_fields->m_button;
    auto buttonGlobalCoords = btn->convertToWorldSpace(btn->getPosition());
    auto buttonCoordsOnEditorUI = this->convertToNodeSpace(buttonGlobalCoords);

    debugLabel->setScale(.5f * btn->getScale());
    // place debug label right under my button
    debugLabel->setPosition(buttonCoordsOnEditorUI - 
        ccp(0, btn->getContentSize().height / 2 + 10));
    debugLabel->setOpacity(0);
    debugLabel->setID("twt-debug-label");

    this->addChild(debugLabel, 999);
    m_fields->m_debugLabel = debugLabel;
}

void MyEditorUI::initButtons() {
    // large button
    auto largeBtnSprite = CCSprite::createWithSpriteFrameName("TWT_tool_off.png"_spr);
    largeBtnSprite->setScale(.91f);
    auto largeBtn = CCMenuItemSpriteExtra::create(
        largeBtnSprite, this, menu_selector(MyEditorUI::onMainButton));
    m_fields->m_button = largeBtn;
    auto twtMenu = CCMenu::create();
    // info button
    if (!Mod::get()->template getSettingValue<bool>("hide-info-button")) { 
        auto smallBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"), this,
            menu_selector(MyEditorUI::onInfoButton));
        twtMenu->addChild(smallBtn);
        smallBtn->setZOrder(2);
    }
    // mode selection buttons
    // auto setupBtnSpr = ButtonSprite::create("Set up");
    // setupBtnSpr->setScale(.5);
    // auto setupBtn = CCMenuItemSpriteExtra::create(
    //     setupBtnSpr, this, menu_selector(MyEditorUI::onModeChanged));
    // setupBtn->setTag(Mode::SetupMode);

    // auto previewBtnSpr = ButtonSprite::create("Preview");
    // previewBtnSpr->setScale(.5);
    // auto previewBtn = CCMenuItemSpriteExtra::create(
    //     previewBtnSpr, this, menu_selector(MyEditorUI::onModeChanged));
    // previewBtn->setTag(Mode::PreviewMode);

    // large button 2
    auto largeBtn2Sprite = CCSprite::createWithSpriteFrameName("TWT_tool_off.png"_spr); // todo: other sprite
    largeBtn2Sprite->setScale(.91f);
    auto largeBtn2 = CCMenuItemSpriteExtra::create(
        largeBtn2Sprite, this, menu_selector(MyEditorUI::onMainButton)); // todo: other selector

    // menu
    twtMenu->setContentWidth(largeBtn->getContentWidth() + largeBtn2->getContentWidth());
    twtMenu->setContentHeight(largeBtn->getContentHeight());
    twtMenu->setID("twt-menu");
    
    twtMenu->addChild(largeBtn);
    largeBtn->setPosition(largeBtn->getContentSize() / 2);

    twtMenu->addChild(largeBtn2);
    largeBtn2->setPosition(largeBtn2->getContentSize() / 2 + ccp(largeBtn2->getContentWidth() * 1.1, 0));

    // twtMenu->addChild(setupBtn);
    // setupBtn->setPosition(ccp(largeBtn->getContentSize().width + setupBtn->
    //     getContentSize().width / 2, largeBtn->getContentHeight() * .75));

    // twtMenu->addChild(previewBtn);
    // previewBtn->setPosition(ccp(largeBtn->getContentSize().width + previewBtn->
    //     getContentSize().width / 2, largeBtn->getContentHeight() * .25));

    auto undoMenu = this->getChildByID("undo-menu");
    undoMenu->setContentSize(undoMenu->getContentSize() + ccp(50, 0));
    undoMenu->setPositionX(undoMenu->getPositionX() + 25);
    undoMenu->addChild(twtMenu);
    undoMenu->updateLayout();

    // slider
    if (Mod::get()->template getSettingValue<bool>("move-slider")) { 
        auto slider = this->getChildByID("position-slider");
        slider->setPositionX(slider->getPositionX() + 50);
    }
}

// set keybinds 

#ifdef GEODE_IS_WINDOWS 

$execute {
    using namespace keybinds;

    BindManager::get()->registerBindable({
        "twt-pan-in-the-editor"_spr,
        "Pan Editor",                 
        "Allows you to pan in the editor if you have this tool enabled.",
        {Keybind::create(KEY_Space)},
        "Triggers Wiring Tool"
    });
    BindManager::get()->registerBindable({
        "twt-activate-tool"_spr, 
        "Turn on/off the tool",  
        "Enables/disables the tool.",
        {Keybind::create(KEY_H)},
        "Triggers Wiring Tool"
    });
}

#endif
