#include "EditLogic.hpp"
#include "Utils.hpp"
#include "../EditorUI.hpp"

// UPPER MENU

struct UpperMenuButtonParameters : public CCObject {
    Variant m_config;
    UpperMenuButtonParameters(Variant const& config) : m_config(config) {
        this->autorelease();
    }
};

void EditLogic::createUpperMenu(const std::vector<Variant>& configuration, bool selectFirst, std::set<srcObjType> commonType) {
    // remove old menu
    if (m_upperMenu) m_upperMenu->removeFromParent();
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
    m_upperMenu = menu;
    // add buttons
    for (unsigned i = 0; i < configuration.size(); i++) {
        auto btnNode = CCNode::create();
        std::string suffix = commonType.contains(configuration[i].m_srcObjType) ? "" : "*"; 
        auto label = CCLabelBMFont::create((configuration[i].m_name + suffix).c_str(), "bigFont.fnt");
        label->setScale(0.5f);
        label->setAnchorPoint(ccp(0, 0));
        btnNode->addChild(label);
        btnNode->setContentSize(label->getScaledContentSize());

        auto btn = CCMenuItemSpriteExtra::create(
            btnNode, this, menu_selector(EditLogic::upperMenuActionListener));
        btn->setTag(i);
        btn->setUserObject(new UpperMenuButtonParameters(configuration[i]));
        menu->addChild(btn);

        if (i == 0 && selectFirst) upperMenuActionListener(btn);
    }
    menu->updateLayout();
    m_drawingLayer->addChild(m_upperMenu);
    m_upperMenu->setPosition(m_objectTarget->getPosition() + ccp(5, 15));
}

// listens to button clicking in upper menu and sets m_config
void EditLogic::upperMenuActionListener(CCObject * sender) {
    auto button = static_cast<CCMenuItemSpriteExtra*>(sender);
    if (button->getChildByID("marker")) return;
    // clear old marker
    auto menu = m_upperMenu;
    auto buttons = menu->getChildren();
    for (unsigned i = 0; i < buttons->count(); i++) {
        auto btn = static_cast<CCMenuItemSpriteExtra*>(buttons->objectAtIndex(i));
        btn->removeChildByID("marker");
    }
    { // add new marker
        auto marker = CCLabelBMFont::create("<", "bigFont.fnt");
        marker->setAnchorPoint(ccp(0, 0));
        marker->setPosition(ccp(button->getContentSize().width + 1, 0));
        marker->setScale(.5f);
        marker->setID("marker");
        button->addChild(marker);
    }
    // update m_globalConfig and maybe create new lower menu
    auto config = static_cast<UpperMenuButtonParameters*>(button->getUserObject())->m_config;
    bool lowerMenuTypeChanged = !m_globalConfig.m_isFinished || 
        config.getLowerMenuType() != m_globalConfig.m_variant.getLowerMenuType();
    if (lowerMenuTypeChanged || m_editorInstance->m_fields->m_modSettings.m_showOldVariant) {
        // then we need new lower menu
        m_globalConfig.m_variant = config;
        // reset source objects
        for (unsigned i = 0; i < m_objectsSource->count(); i++) {
            auto obj = static_cast<GameObject*>(m_objectsSource->objectAtIndex(i));
            auto objDefaultAttributes = static_cast<TWTObjCopy*>(m_objectsSourceCopy->objectAtIndex(i));
            myPasteObjectProps(objDefaultAttributes, obj); // restore initial groups
        }
        createLowerMenuForVariant(config);

        m_drawingLayer->addChild(m_lowerMenu);
        m_lowerMenu->setPosition(m_objectTarget->getPosition() + ccp(5, -15));

        if (!m_globalConfig.m_isFinished) {
            m_globalConfig.m_isFinished = true;
            applyToolConfig(lowerMenuTypeChanged);
            // otherwise it was already applied when creating the lower menu
        }
    } else {
        m_globalConfig.m_variant = config;
        applyToolConfig(false);
    }
}


// LOWER MENU

struct LowerMenuButtonParameters : public CCObject {
    std::pair<std::string, int> m_config;
    LowerMenuButtonParameters(std::pair<std::string, int> const& config) : m_config(config) {
        this->autorelease();
    }
};

void EditLogic::createLowerMenuForVariant(Variant variant) {
    // color or group or item or collision block or ...
    auto menuType = variant.getLowerMenuType().first;
    // consider source objects only of this type
    auto forObjectsType = variant.getLowerMenuType().second;
    auto filteredObjects = filterObjectsByType(forObjectsType, m_objectsSource, true, m_editorInstance);
    m_objectsSourceFiltered = filteredObjects;

    // check which key we are changing
    int oldVal = 0;
    if (m_editorInstance->m_fields->m_modSettings.m_showOldVariant && !variant.m_appendNotOverride) {
        std::stringstream ss(variant.m_triggerConfigString);
        std::string key, val;
        while (std::getline(ss, key, ',') && std::getline(ss, val, ',')) {
            if (val == "g") break;
        }
        auto kvObj = objectToKeyVal(m_objectTargetInitial);
        oldVal = kvObj.contains(key) ? std::stoi(kvObj[key]) : 0;
    }

    std::vector<std::pair<std::string, int>> lowerMenuConfig;
    switch (menuType) {
        case lowerMenuType::selectGroup: {
            auto commonGroups = getObjectsCommonGroups(filteredObjects);
            for (unsigned i = 0; i < commonGroups.size(); i++) {
                lowerMenuConfig.push_back({std::format("group {}", commonGroups.at(i)), commonGroups.at(i)});
            }
            if (isNewGroupPossible(filteredObjects)) {
                int nextFree = m_globalConfig.m_nextFreeGroup;
                lowerMenuConfig.push_back({std::format("next ({})", nextFree), nextFree});
            }
            if (oldVal) lowerMenuConfig.push_back({std::format("old ({})", oldVal), oldVal});
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
        case lowerMenuType::selectItem: {
            auto allItemIds = getItemsAllIds(filteredObjects);
            auto nextFreeItem = m_editorInstance->m_fields->m_modSettings.m_fixedNextFreeItem ?
                getNextFreeItemFixed() : LevelEditorLayer::get()->getNextFreeItemID(nullptr);
            for (unsigned i = 0; i < allItemIds.size(); i++) {
                if (allItemIds.at(i)) {
                    lowerMenuConfig.push_back({std::format("item {}", allItemIds.at(i)), allItemIds.at(i)});
                }
            }
            lowerMenuConfig.push_back({std::format("next ({})", nextFreeItem), nextFreeItem});
            if (oldVal) lowerMenuConfig.push_back({std::format("old ({})", oldVal), oldVal});
            lowerMenuConfig.push_back({"None", -1});
            break;
        }
        case lowerMenuType::selectCollision: {
            auto allCollisionIds = getCollisionsAllIds(filteredObjects);
            auto nextFreeCollision = getNextFreeBlock();
            for (unsigned i = 0; i < allCollisionIds.size(); i++) {
                if (allCollisionIds.at(i)) {
                    lowerMenuConfig.push_back({std::format("block {}", allCollisionIds.at(i)), allCollisionIds.at(i)});
                }
            }
            lowerMenuConfig.push_back({std::format("next ({})", nextFreeCollision), nextFreeCollision});
            if (oldVal) lowerMenuConfig.push_back({std::format("old ({})", oldVal), oldVal});
            lowerMenuConfig.push_back({"None", -1});
            break;
        }
        case lowerMenuType::selectCopyGroup: {
            if (m_objectTarget->m_groups) {
                for (unsigned i = 0; i < 10; i++) {
                    short group = m_objectTarget->m_groups->at(i);
                    if (!group) break;
                    lowerMenuConfig.push_back({std::format("group {}", group), group});
                }
            }
            lowerMenuConfig.push_back({"None", -1});
            break;
        }
        default: break;
    }
    createLowerMenu(lowerMenuConfig, true);
    log::debug("Created new lower menu");
}

void EditLogic::createLowerMenu(const std::vector<std::pair<std::string, int>>& configuration, bool selectFirst) {
    // remove old menu
    if (m_lowerMenu) m_lowerMenu->removeFromParent();
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
    m_lowerMenu = menu;
    // add buttons
    for (unsigned i = 0; i < configuration.size(); i++) {
        auto btnNode = CCNode::create();
        auto label = CCLabelBMFont::create(configuration[i].first.c_str(), "bigFont.fnt");
        label->setScale(0.5f);
        label->setAnchorPoint({0.f, 0.f});
        btnNode->addChild(label);
        btnNode->setContentSize(label->getScaledContentSize());

        auto btn = CCMenuItemSpriteExtra::create(
            btnNode, this, menu_selector(EditLogic::lowerMenuActionListener));
        btn->setTag(i);
        btn->setUserObject(new LowerMenuButtonParameters(configuration[i]));

        menu->addChild(btn);

        if (i == 0 && selectFirst) lowerMenuActionListener(btn);
    }
    menu->updateLayout();
}

// listens to button clicking in lower menu and sets m_config
void EditLogic::lowerMenuActionListener(CCObject * sender) {
    auto menu = m_lowerMenu;
    if (static_cast<CCMenuItemSpriteExtra*>(sender)->getChildByID("marker")) return;
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
    m_globalConfig.m_group = config.second;

    applyToolConfig(true);
}