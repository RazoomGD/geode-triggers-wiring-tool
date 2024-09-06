#include "EditorUI.hpp"

// UPPER MENU

struct UpperMenuButtonParameters : public CCObject {
    Variant m_config;
    UpperMenuButtonParameters(Variant const& config) : m_config(config) {
        this->autorelease();
    }
};

void MyEditorUI::createUpperMenu(const std::vector<Variant>& configuration, bool selectFirst, srcObjType commonType) {
    // remove old menu
    if (m_fields->m_upperMenu) m_fields->m_upperMenu->removeFromParent();
    // create new menu
    auto menu = CCMenu::create();
    menu->setLayout(ColumnLayout::create()
                        ->setGap(0.f)
                        ->setGrowCrossAxis(true)
                        ->setAxisAlignment(AxisAlignment::Start)
                        ->setCrossAxisLineAlignment(AxisAlignment::End));
    menu->setContentSize(CCSizeMake(0, 200));
    menu->setAnchorPoint(ccp(1, 0));
    menu->setTouchPriority(-120);
    m_fields->m_upperMenu = menu;
    // add buttons
    for (unsigned i = 0; i < configuration.size(); i++) {
        auto btnNode = CCNode::create();
        std::string suffix = (configuration[i].m_srcObjType != commonType && 
            configuration[i].m_srcObjType != srcObjType::any) ? "*" : "";
        auto label = CCLabelBMFont::create((configuration[i].m_name + suffix).c_str(), "bigFont.fnt");
        label->setScale(0.5f);
        label->setAnchorPoint(ccp(0, 0));
        btnNode->addChild(label);
        btnNode->setContentSize(label->getScaledContentSize());

        auto btn = CCMenuItemSpriteExtra::create(
            btnNode, this, menu_selector(MyEditorUI::upperMenuActionListener));
        btn->setTag(i);
        btn->setUserObject(new UpperMenuButtonParameters(configuration[i]));
        menu->addChild(btn);

        if (i == 0 && selectFirst) upperMenuActionListener(btn);
    }
    menu->updateLayout();
}

// listens to button clicking in upper menu and sets m_config
void MyEditorUI::upperMenuActionListener(CCObject * sender) {
    auto menu = m_fields->m_upperMenu;
    // clear old marker
    auto buttons = menu->getChildren();
    for (unsigned i = 0; i < buttons->count(); i++) {
        auto btn = static_cast<CCMenuItemSpriteExtra*>(buttons->objectAtIndex(i));
        btn->removeChildByID("marker");
    }
    // add new marker
    auto button = static_cast<CCMenuItemSpriteExtra*>(sender);
    auto marker = CCLabelBMFont::create("<", "bigFont.fnt");
    marker->setAnchorPoint(ccp(0, 0));
    marker->setPosition(ccp(button->getContentSize().width + 1, 0));
    marker->setScale(.5f);
    marker->setID("marker");
    button->addChild(marker);
    // update m_globalConfig and maybe create new lower menu
    auto config = static_cast<UpperMenuButtonParameters*>(button->getUserObject())->m_config;
    if (!m_fields->m_globalConfig.m_isFinished || 
        config.getLowerMenuType() != m_fields->m_globalConfig.m_variant.getLowerMenuType()) {
        // then we need new lower menu
        m_fields->m_globalConfig.m_variant = config;
        createLowerMenuForVariant(config);

        auto lowerMenu = m_fields->m_lowerMenu;
        m_fields->m_drawingLayer->addChild(lowerMenu);
        lowerMenu->setPosition(m_fields->m_objectTarget->getPosition() + ccp(5, -15));

        m_fields->m_globalConfig.m_isFinished = true;
    } else {
        m_fields->m_globalConfig.m_variant = config;
    }
    applyToolConfig();
}


// LOWER MENU

struct LowerMenuButtonParameters : public CCObject {
    std::pair<std::string, int> m_config;
    LowerMenuButtonParameters(std::pair<std::string, int> const& config) : m_config(config) {
        this->autorelease();
    }
};

void MyEditorUI::createLowerMenuForVariant(Variant variant) {
    // color or group or item or collision block or ...
    auto menuType = variant.getLowerMenuType().first;
    // consider source objects only of this type
    auto forObjectsType = variant.getLowerMenuType().second;
    auto filteredObjects = filterObjectsByType(forObjectsType, m_fields->m_objectsSource, true);
    m_fields->m_objectsSourceFiltered = filteredObjects;

    std::vector<std::pair<std::string, int>> lowerMenuConfig;
    switch (menuType) {
        case lowerMenuType::selectGroup: {
            auto newGroupPossible = isNewGroupPossible(filteredObjects);
            if (newGroupPossible) {
                int nextFree = LevelEditorLayer::get()->getNextFreeGroupID(nullptr);
                lowerMenuConfig.push_back({std::format("next ({})", nextFree), nextFree});
                // lowerMenuConfig.push_back({"next free", nextFree});
            }
            auto commonGroups = getObjectsCommonGroups(filteredObjects);
            for (unsigned i = 0; i < commonGroups.size(); i++) {
                lowerMenuConfig.push_back({std::format("group {}", commonGroups.at(i)), commonGroups.at(i)});
            }
            lowerMenuConfig.push_back({"None", -1});
            break;
        }
        case lowerMenuType::selectColor: {
            auto allColors = getObjectsAllColors(filteredObjects);
            auto maybeCommonBaseCol = getCommonBaseColor(filteredObjects);
            auto maybeCommonDetailCol = getCommonDetailColor(filteredObjects);
            if (maybeCommonBaseCol && *maybeCommonBaseCol != 0) {
                auto col = *maybeCommonBaseCol;
                if (colorIdName.contains(col)) 
                    lowerMenuConfig.push_back({std::format("(base) {}", colorIdName.at(col)), col});
                else lowerMenuConfig.push_back({std::format("(base) col {}", col), col});
            }
            if (maybeCommonDetailCol && *maybeCommonDetailCol != 0) {
                auto col = *maybeCommonDetailCol;
                if (colorIdName.contains(col)) 
                    lowerMenuConfig.push_back({std::format("(detail) {}", colorIdName.at(col)), col});
                else lowerMenuConfig.push_back({std::format("(detail) col {}", col), col});
            }
            for (auto col : allColors) {
                if (maybeCommonBaseCol && col == *maybeCommonBaseCol) continue;
                if (maybeCommonDetailCol && col == *maybeCommonDetailCol) continue;
                if (colorIdName.contains(col)) lowerMenuConfig.push_back({colorIdName.at(col), col});
                else if (col != 0) lowerMenuConfig.push_back({std::format("col {}", col), col});
            }
            lowerMenuConfig.push_back({"None", -1});
            break;
        }
        default: break;
    }
    createLowerMenu(lowerMenuConfig, true);
}

void MyEditorUI::createLowerMenu(const std::vector<std::pair<std::string, int>>& configuration, bool selectFirst) {
    // remove old menu
    if (m_fields->m_lowerMenu) m_fields->m_lowerMenu->removeFromParent();
    // create new menu
    auto menu = CCMenu::create();
    menu->setLayout(ColumnLayout::create()
                        ->setGap(0.f)
                        ->setGrowCrossAxis(true)
                        ->setAxisAlignment(AxisAlignment::End)
                        ->setAxisReverse(true)
                        ->setCrossAxisLineAlignment(AxisAlignment::End));
    menu->setContentSize(CCSizeMake(0, 200));
    menu->setAnchorPoint(ccp(1, 1));
    menu->setTouchPriority(-120);
    m_fields->m_lowerMenu = menu;
    // add buttons
    for (unsigned i = 0; i < configuration.size(); i++) {
        auto btnNode = CCNode::create();
        auto label = CCLabelBMFont::create(configuration[i].first.c_str(), "bigFont.fnt");
        label->setScale(0.5f);
        label->setAnchorPoint({0.f, 0.f});
        btnNode->addChild(label);
        btnNode->setContentSize(label->getScaledContentSize());

        auto btn = CCMenuItemSpriteExtra::create(
            btnNode, this, menu_selector(MyEditorUI::lowerMenuActionListener));
        btn->setTag(i);
        btn->setUserObject(new LowerMenuButtonParameters(configuration[i]));

        menu->addChild(btn);

        if (i == 0 && selectFirst) lowerMenuActionListener(btn);
    }
    menu->updateLayout();
}

// listens to button clicking in lower menu and sets m_config
void MyEditorUI::lowerMenuActionListener(CCObject * sender) {
    auto menu = m_fields->m_lowerMenu;
    // remove old marker
    auto buttons = menu->getChildren();
    for (unsigned i = 0; i < buttons->count(); i++) {
        auto btn = static_cast<CCMenuItemSpriteExtra*>(buttons->objectAtIndex(i));
        btn->removeChildByID("marker");
    }
    // add new marker
    auto button = static_cast<CCMenuItemSpriteExtra*>(sender);
    auto marker = CCLabelBMFont::create("<", "bigFont.fnt");
    marker->setAnchorPoint(ccp(0, 0));
    marker->setPosition(ccp(button->getContentSize().width + 1, 0));
    marker->setScale(.5f);
    marker->setID("marker");
    button->addChild(marker);

    // update m_globalConfig
    auto config = static_cast<LowerMenuButtonParameters*>(button->getUserObject())->m_config;
    m_fields->m_globalConfig.m_group = config.second;

    applyToolConfig();
}