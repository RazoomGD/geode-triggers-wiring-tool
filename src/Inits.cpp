#include "EditorUI.hpp"

void MyEditorUI::initDebugLabel() {
    auto debugLabel = CCLabelBMFont::create("", "bigFont.fnt");
    // btn one
    auto btn = m_fields->m_editButton;
    auto buttonGlobalCoords = btn->convertToWorldSpace(btn->getPosition());
    auto buttonCoordsOnEditorUI = this->convertToNodeSpace(buttonGlobalCoords);
    // btn two
    auto btn2 = m_fields->m_previewButton;
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
    auto twtMenu = CCMenu::create();

    // edit button
    auto editBtnSprite = CCSprite::createWithSpriteFrameName("TWT_tool_off.png"_spr);
    editBtnSprite->setScale(.91f);
    auto editBtn = CCMenuItemSpriteExtra::create(
        editBtnSprite, this, menu_selector(MyEditorUI::onMainButtonWrapper));
    twtMenu->addChild(editBtn);
    editBtn->setPosition(editBtn->getContentSize() / 2);
    m_fields->m_editButton = editBtn;

    // preview button 2
    auto previewBtnSprite = CCSprite::createWithSpriteFrameName("TWT_preview_tool_off.png"_spr);
    previewBtnSprite->setScale(.91f);
    auto previewBtn = CCMenuItemSpriteExtra::create(
        previewBtnSprite, this, menu_selector(MyEditorUI::onPreviewButtonWrapper));
    twtMenu->addChild(previewBtn);
    previewBtn->setPosition(previewBtn->getContentSize() / 2 + ccp(previewBtn->getContentWidth() * 1.1, 0));
    m_fields->m_previewButton = previewBtn;
    
    // info buttons
    if (!Mod::get()->template getSettingValue<bool>("hide-info-button")) { 
        // edit mode
        auto editBtnInfoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        editBtnInfoSpr->setScale(.77f);
        auto editBtnInfo = CCMenuItemSpriteExtra::create(editBtnInfoSpr, this,
            menu_selector(MyEditorUI::onInfoButtonWrapper));
        twtMenu->addChild(editBtnInfo);
        editBtnInfo->setZOrder(2);
        // preview mode
        auto previewBtnInfoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        previewBtnInfoSpr->setScale(.77f);
        auto previewBthInfo = CCMenuItemSpriteExtra::create(previewBtnInfoSpr, this,
            menu_selector(MyEditorUI::onInfoButtonWrapper2));
        twtMenu->addChild(previewBthInfo);
        previewBthInfo->setZOrder(2);
        previewBthInfo->setPositionX(previewBtn->getPositionX() - editBtn->getPositionX());
    }

    // menu
    twtMenu->setContentWidth(editBtn->getContentWidth() + previewBtn->getContentWidth());
    twtMenu->setContentHeight(editBtn->getContentHeight());
    twtMenu->setID("twt-menu");

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
    // 'H' for edit button
    this->template addEventListener<keybinds::InvokeBindFilter>(
        [=](keybinds::InvokeBindEvent* event) {
            if (event->isDown()) {
                m_fields->m_editLogic->onMainButton(m_fields->m_editButton);
            }
            return ListenerResult::Propagate;
        },
        "twt-activate-edit-tool"_spr);
    // 'J' for preview button
    this->template addEventListener<keybinds::InvokeBindFilter>(
        [=](keybinds::InvokeBindEvent* event) {
            if (event->isDown()) {
                m_fields->m_previewLogic->onPreviewButton(m_fields->m_previewButton);
            }
            return ListenerResult::Propagate;
        },
        "twt-activate-preview-tool"_spr);
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
        "twt-activate-edit-tool"_spr, 
        "Turn on/off edit mode",  
        "Enables/disables edit mode.",
        {Keybind::create(KEY_H)},
        "Triggers Wiring Tool"
    });
    BindManager::get()->registerBindable({
        "twt-activate-preview-tool"_spr, 
        "Turn on/off preview mode",  
        "Enables/disables preview mode.",
        {Keybind::create(KEY_J)},
        "Triggers Wiring Tool"
    });
}

#endif
