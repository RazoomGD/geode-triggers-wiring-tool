#include "EditorUI.hpp"

void MyEditorUI::initDebugLabel() {
    auto debugLabel = CCLabelBMFont::create("", "bigFont.fnt");
    // btn one
    auto btn = m_fields->m_button;
    auto buttonGlobalCoords = btn->convertToWorldSpace(btn->getPosition());
    auto buttonCoordsOnEditorUI = this->convertToNodeSpace(buttonGlobalCoords);
    // btn two
    auto btn2 = m_fields->m_button2;
    auto buttonGlobalCoords2 = btn->convertToWorldSpace(btn2->getPosition());
    auto buttonCoordsOnEditorUI2 = this->convertToNodeSpace(buttonGlobalCoords2);

    debugLabel->setScale(.5f * btn->getScale());
    // place debug label right under my buttons
    debugLabel->setPosition((buttonCoordsOnEditorUI + buttonCoordsOnEditorUI2) / 2 - 
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
        largeBtnSprite, this, menu_selector(MyEditorUI::onMainButtonWrapper));
    m_fields->m_button = largeBtn;
    auto twtMenu = CCMenu::create();
    // info button
    if (!Mod::get()->template getSettingValue<bool>("hide-info-button")) { 
        auto smallBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"), this,
            menu_selector(EditLogic::onInfoButton));
        twtMenu->addChild(smallBtn);
        smallBtn->setZOrder(2);
    }

    // large button 2
    auto largeBtn2Sprite = CCSprite::createWithSpriteFrameName("TWT_preview_tool_off.png"_spr);
    largeBtn2Sprite->setScale(.91f);
    auto largeBtn2 = CCMenuItemSpriteExtra::create(
        largeBtn2Sprite, this, menu_selector(MyEditorUI::onPreviewButton));
    m_fields->m_button2 = largeBtn2;

    // menu
    twtMenu->setContentWidth(largeBtn->getContentWidth() + largeBtn2->getContentWidth());
    twtMenu->setContentHeight(largeBtn->getContentHeight());
    twtMenu->setID("twt-menu");
    
    twtMenu->addChild(largeBtn);
    largeBtn->setPosition(largeBtn->getContentSize() / 2);

    twtMenu->addChild(largeBtn2);
    largeBtn2->setPosition(largeBtn2->getContentSize() / 2 + ccp(largeBtn2->getContentWidth() * 1.1, 0));

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
void MyEditorUI::setKeybinds() {
    #ifdef GEODE_IS_WINDOWS 
    this->template addEventListener<keybinds::InvokeBindFilter>(
        [=](keybinds::InvokeBindEvent* event) {
            if (event->isDown()) {
                m_fields->m_editLogic->onMainButton(m_fields->m_button);
            }
            return ListenerResult::Propagate;
        },
        "twt-activate-tool"_spr);
    #endif
}

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
