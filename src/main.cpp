#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <utility>
#include <cstring>

#ifdef GEODE_IS_WINDOWS 
#include <geode.custom-keybinds/include/Keybinds.hpp>
#endif

using namespace geode::prelude;

#include "configuration.hpp"

class $modify(MyEditorUI, EditorUI) {
    struct ToolConfig {
        std::string targetStr;
        sourceFuncType sourceFunc;
        short group;
        bool isFinished = false;
    };

    struct Fields {
        CCMenuItemSpriteExtra* m_button = nullptr;
        CCLabelBMFont* m_debugLabel = nullptr;
        CCDrawNode* m_drawingLayer = nullptr;
        CCPoint m_lineStart;  // both are editor coordinates
        CCPoint m_lineEnd;
        std::pair<CCPoint, CCPoint> m_sourceRectangle;
        Ref<CCArray> m_objectsSource = nullptr;  // array of selected objects
        Ref<GameObject> m_objectTarget = nullptr;  // end object
        Ref<CCArray> m_objectsSourceCopy = nullptr;
        std::string m_objectTargetCopy;
        Ref<CCMenu> m_upperMenu = nullptr;
        Ref<CCMenu> m_lowerMenu = nullptr;
        bool m_ctrlModifierEnabled = false;
        bool m_buttonIsActivated = false;
        bool m_interfaceIsVisible = false;
        bool m_panEditor = false;
        ToolConfig m_globalConfig;
    };

    void myCopyGroups(GameObject* from, GameObject* to) {
        // groups
        if (!to->m_groups) to->addToGroup(1);
        if (from->m_groups) {
            std::memcpy(to->m_groups, from->m_groups, sizeof(short) * 10);
        } else {
            to->m_groups->fill(0);
        }
        to->m_groupCount = from->m_groupCount;
    }

    bool toolIsActivated() {
        #ifdef GEODE_IS_WINDOWS 
            return ((m_fields->m_ctrlModifierEnabled && 
                CCKeyboardDispatcher::get()->getControlKeyPressed()) || 
                m_fields->m_buttonIsActivated) && !m_fields->m_panEditor;
        #else
            return m_fields->m_buttonIsActivated;
        #endif
    }

    // ---------------------------- inits --------------------------------

    void initDrawingLayer() {
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

    void initDebugLabel() {
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

    void initButtons() {
        auto bigBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("TWT_tool_off.png"_spr), this,
            menu_selector(MyEditorUI::onMyButton));
        m_fields->m_button = bigBtn;
        auto twtMenu = CCMenu::create();
        if (!Mod::get()->getSettingValue<bool>("hide-info-button")) {
            auto smallBtn = CCMenuItemSpriteExtra::create(
                CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"), this,
                menu_selector(MyEditorUI::onInfoButton));
            twtMenu->addChild(smallBtn);
            smallBtn->setZOrder(2);
        }
        twtMenu->setContentWidth(bigBtn->getContentWidth());
        twtMenu->setContentHeight(bigBtn->getContentHeight());
        twtMenu->setID("twt-menu");
        
        twtMenu->addChild(bigBtn);
        bigBtn->setPosition(twtMenu->getContentSize() / 2);

        auto undoMenu = this->getChildByID("undo-menu");
        undoMenu->addChild(twtMenu);
        undoMenu->updateLayout();
    }

    void setKeybinds() {
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
                    this->onMyButton(nullptr);
                }
                return ListenerResult::Propagate;
            },
            "twt-activate-tool"_spr);
        #endif
    }

    // ------------------------------ actions -------------------------------

    void showDebugText(std::string text, float timeSec=0.5) {
        if (!m_fields->m_debugLabel) initDebugLabel();
        auto label = m_fields->m_debugLabel;
        label->setString(text.c_str());
        label->setOpacity(255);
        label->stopAllActions();
        label->runAction(CCSequence::create(
            CCDelayTime::create(timeSec), CCFadeOut::create(timeSec), nullptr));
    }

    void onMyButton(CCObject*) {
        m_fields->m_buttonIsActivated = !m_fields->m_buttonIsActivated;
        auto btn = m_fields->m_button;
        if (m_fields->m_buttonIsActivated) {
            btn->setSprite(CCSprite::createWithSpriteFrameName("TWT_tool_on.png"_spr));
            showDebugText("Tool is on");
        } else {
            btn->setSprite(CCSprite::createWithSpriteFrameName("TWT_tool_off.png"_spr));
            m_fields->m_panEditor = false;
            resetTool();
            showDebugText("Tool is off");
        }
    }

    void onInfoButton(CCObject*) {
        FLAlertLayer::create(
            "Color Example",
            "text",
            "OK"
        )->show();
    }

    // --------------------------- overwritten functions ---------------------

    $override bool init(LevelEditorLayer * layer) {
        if (!EditorUI::init(layer)) return false;
        initButtons();
        setKeybinds();
        #ifdef GEODE_IS_WINDOWS
            m_fields->m_ctrlModifierEnabled = Mod::get()->getSettingValue<bool>("control-key-modifier");
        #endif
        this->schedule(SEL_SCHEDULE(&MyEditorUI::controlTargetObjectCallback), 0.f);
        return true;
    }

    $override bool ccTouchBegan(CCTouch * touch, CCEvent * event) {
        if (toolIsActivated()) {
            if (m_fields->m_interfaceIsVisible) {
                resetTool();
            }
            auto startedUsingTool = handleTouchStart(touch);
            if (startedUsingTool) {
                m_fields->m_interfaceIsVisible = true;
                return true;
            } else {
                return EditorUI::ccTouchBegan(touch, event);
            }
        } else {
            return EditorUI::ccTouchBegan(touch, event);
        }
    }

    $override void ccTouchMoved(CCTouch * touch, CCEvent * event) {
        if (toolIsActivated() && m_fields->m_interfaceIsVisible) {
            handleTouchMiddle(touch);
        } else {
            EditorUI::ccTouchMoved(touch, event);
            if (m_fields->m_interfaceIsVisible && !m_fields->m_objectTarget) {
                resetTool();
            }
        }
    }

    $override void ccTouchEnded(CCTouch * touch, CCEvent * event) {
        if (toolIsActivated() && m_fields->m_interfaceIsVisible) {
            handleTouchEnd(touch, true);
        } else {
            EditorUI::ccTouchEnded(touch, event);
            if (m_fields->m_interfaceIsVisible && !m_fields->m_objectTarget) {
                resetTool();
            }
        }
    }

    // --------------------------- Touch Handlers -----------------------------

    bool handleTouchStart(const CCTouch * const touch) {
        m_fields->m_lineStart = screenToEditorLayerPosition(touch->getLocationInView());
        m_fields->m_lineEnd = screenToEditorLayerPosition(touch->getLocationInView());

        auto preSelected = EditorUI::getSelectedObjects();

        // if we have previously selected objects
        // we check if we've started line from them
        bool useSelectedFlag = false;
        if (preSelected->count()) {
            for (unsigned i = 0; i < preSelected->count(); i++) {
                auto obj = static_cast<GameObject*>(preSelected->objectAtIndex(i));
                auto objBox = obj->boundingBox();
                auto click = m_fields->m_lineEnd;
                if (  // check if we've started line from selected object
                    click.x >= objBox.getMinX() && click.x <= objBox.getMaxX() &&
                    click.y >= objBox.getMinY() && click.y <= objBox.getMaxY()) {
                    useSelectedFlag = true;
                    break;
                }
            }
        }
        // set source object(s)
        CCArray* sourceObjects = nullptr;
        if (useSelectedFlag) {
            sourceObjects = preSelected;
        } else {
            auto startObj = LevelEditorLayer::get()->objectAtPosition(m_fields->m_lineEnd);
            if (startObj) {
                EditorUI::selectObject(startObj, false);
                sourceObjects = EditorUI::getSelectedObjects();
            } else if (preSelected->count()) {
                sourceObjects = preSelected;
            }
        }

        if (sourceObjects) {
            EditorUI::deselectAll();
            for (unsigned i = 0; i < sourceObjects->count(); i++) {
                auto obj = static_cast<GameObject*>(sourceObjects->objectAtIndex(i));
                obj->selectObject(ccc3(255, 0, 255));
            }
            m_fields->m_objectsSource = sourceObjects;
            showDebugText(std::format("Selected objects: {}", sourceObjects->count()));
            // get source rectangle
            int leftX = INT_MAX; 
            int topY = INT_MIN;
            int rightX = INT_MIN;
            int bottomY = INT_MAX;
            for (unsigned i = 0; i < sourceObjects->count(); i++) {
                auto obj = static_cast<GameObject*>(sourceObjects->objectAtIndex(i));
                auto bound = obj->boundingBox();
                if (bound.getMaxX() > rightX) rightX = bound.getMaxX();
                if (bound.getMinX() < leftX) leftX = bound.getMinX();
                if (bound.getMaxY() > topY) topY = bound.getMaxY();
                if (bound.getMinY() < bottomY) bottomY = bound.getMinY();
            }
            auto topLeft = ccp(leftX, topY);
            auto bottomRight = ccp(rightX, bottomY);
            m_fields->m_sourceRectangle.first = topLeft - ccp(3, -3);
            m_fields->m_sourceRectangle.second = bottomRight + ccp(3, -3);

            m_fields->m_lineStart = calculateLineStartOnRectangle(m_fields->m_lineEnd, m_fields->m_sourceRectangle);
            updateLineAndRect(false);
            return true;
        }
        showDebugText("No objects selected");
        return false;
    }

    void handleTouchMiddle(const CCTouch * const touch) {
        m_fields->m_lineEnd = screenToEditorLayerPosition(touch->getLocationInView());
        m_fields->m_lineStart = calculateLineStartOnRectangle(m_fields->m_lineEnd, m_fields->m_sourceRectangle);
        updateLineAndRect(false);
    }

    void handleTouchEnd(bool select, bool showDebug) {
        if (m_fields->m_objectsSource) {
            auto endObj = LevelEditorLayer::get()->objectAtPosition(m_fields->m_lineEnd);
            if (select && endObj) {
                m_fields->m_lineEnd = endObj->getPosition();
                m_fields->m_lineStart = calculateLineStartOnRectangle(m_fields->m_lineEnd, m_fields->m_sourceRectangle);
                updateLineAndRect(false);
                m_fields->m_objectTarget = endObj;
                handleTargetObject();
            } else {
                if (showDebug) showDebugText("No objects selected");
                resetTool();
            }
        } else {
            resetTool();
        }
    }
    
    // -------------------------- upper menu logic ------------------------------

    struct UpperMenuButtonParameters : public CCObject {
        Variant m_config;
        UpperMenuButtonParameters(Variant const& config) : m_config(config) {
            this->autorelease();
        }
    };

    void createUpperMenu(const std::vector<Variant>& configuration, bool selectFirst) {
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
            auto label = CCLabelBMFont::create(configuration[i].m_name.c_str(), "bigFont.fnt");
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
    void upperMenuActionListener(CCObject * sender) {
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
        // update m_globalConfig
        auto config = static_cast<UpperMenuButtonParameters*>(button->getUserObject())->m_config;
        m_fields->m_globalConfig.targetStr = config.m_triggerConfigString;

        applyToolConfig();
    }

    // ------------------------------- lower menu logic ------------------------------------

    struct LowerMenuButtonParameters : public CCObject {
        //        button-text  id
        std::pair<std::string, int> m_config;
        LowerMenuButtonParameters(std::pair<std::string, int> const& config) : m_config(config) {
            this->autorelease();
        }
    };

    void createLowerMenu(const std::vector<std::pair<std::string, int>>& configuration, bool selectFirst) {
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
    void lowerMenuActionListener(CCObject * sender) {
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
        m_fields->m_globalConfig.group = config.second;

        applyToolConfig();
    }

    // --------------------------- tool main logic -------------------------------

    // this is called only once before each tool use 
    // and meant to set initial configuration and prepare everything
    void handleTargetObject() {
        if (!m_fields->m_objectTarget || !m_fields->m_objectsSource) return;
        auto levelLayer = LevelEditorLayer::get();
        auto targetObj = m_fields->m_objectTarget;
        EditorUI::selectObject(targetObj, true);
        if (EditorUI::getSelectedObjects()->count() > 1) {
            // this may happen if target object has linked objects
            showDebugText("Multiple target objects\naren't supported yet", 1.f);
            return;
        }
        // create and save copy of target object 
        m_fields->m_objectTargetCopy = targetObj->getSaveString(nullptr);
        
        // create and save copy of source objects
        m_fields->m_objectsSourceCopy = CCArray::create();
        for (unsigned i = 0; i < m_fields->m_objectsSource->count(); i++) {
            auto obj = static_cast<GameObject*>(m_fields->m_objectsSource->objectAtIndex(i));
            auto objCopy = GameObject::createWithKey(1);
            myCopyGroups(obj, objCopy);
            m_fields->m_objectsSourceCopy->addObject(objCopy);
        }

        // get configuration for given source and target objects
        auto forTargetObjType = CONFIGURATION.find(targetObj->m_objectID);
        if (forTargetObjType == CONFIGURATION.end()) {
            // we don't have any options for given object (probably because target is not a trigger)
            log::debug("obj not found in map");
            // todo: common config (just copy groups from target to source)
            return;
        } 
        // (else) we have some options for given target object 
        auto forSourceObjType = forTargetObjType->second.find(
            getObjectsCommonType(m_fields->m_objectsSource));
        if (forSourceObjType == forTargetObjType->second.end()) {
            forSourceObjType = forTargetObjType->second.find(srcObjType::any);
            if (forSourceObjType == forTargetObjType->second.end()) {
                //todo
                log::debug("this combination is not supported");
                return;
            }
        }
        m_fields->m_globalConfig.sourceFunc = forSourceObjType->second.m_srcFuncType;

        { // create upper menu
            auto upperMenuConfig = forSourceObjType->second.m_variants;
            if (upperMenuConfig.size() == 0) {
                resetTool();
                return;
            }
            createUpperMenu(upperMenuConfig, true);
            auto upperMenu = m_fields->m_upperMenu;
            m_fields->m_drawingLayer->addChild(upperMenu);
            upperMenu->setPosition(targetObj->getPosition() + ccp(5, 15));
        }

        std::vector<std::pair<std::string, int>> lowerMenuConfig;
        { // get lower menu config (depends on function for source objects)
            switch (m_fields->m_globalConfig.sourceFunc) {
                case sourceFuncType::addGr:
                case sourceFuncType::addGrSM: {
                    auto newGroupPossible = isNewGroupPossible(m_fields->m_objectsSourceCopy);
                    if (newGroupPossible) {
                        int nextFree = levelLayer->getNextFreeGroupID(nullptr);
                        lowerMenuConfig.push_back({std::format("next ({})", nextFree), nextFree});
                        // lowerMenuConfig.push_back({"next free", nextFree});
                    }
                    auto commonGroups = getObjectsCommonGroups(m_fields->m_objectsSourceCopy);
                    for (unsigned i = 0; i < commonGroups.size(); i++) {
                        lowerMenuConfig.push_back({std::format("group {}", commonGroups.at(i)), commonGroups.at(i)});
                    }
                    lowerMenuConfig.push_back({"none", -1});
                    break;
                }
                default: break;
            }
        }

        { // create lower menu
            if (lowerMenuConfig.size() == 0) {
                showDebugText("Impossible assign common\ngroup for objects", 1);
                resetTool();
                return;
            }
            createLowerMenu(lowerMenuConfig, true);
            auto lowerMenu = m_fields->m_lowerMenu;
            m_fields->m_drawingLayer->addChild(lowerMenu);
            lowerMenu->setPosition(targetObj->getPosition() + ccp(5, -15));
        }

        m_fields->m_globalConfig.isFinished = true;
        applyToolConfig();
    }


    void applyToolConfig() {
        if (!m_fields->m_globalConfig.isFinished) return;
        auto targetStr = m_fields->m_globalConfig.targetStr;
        auto sourceFunc = m_fields->m_globalConfig.sourceFunc;
        short group = m_fields->m_globalConfig.group;
        bool resetToDefault = group == -1;
        auto levelLayer = LevelEditorLayer::get();
        
        // source objects
        for (unsigned i = 0; i < m_fields->m_objectsSource->count(); i++) {
            auto obj = static_cast<GameObject*>(m_fields->m_objectsSource->objectAtIndex(i));
            auto objDefaultAttributes = static_cast<GameObject*>(m_fields->m_objectsSourceCopy->objectAtIndex(i));
            myCopyGroups(objDefaultAttributes, obj); // restore initial groups
        }

        if (!resetToDefault) switch (m_fields->m_globalConfig.sourceFunc) {
            case sourceFuncType::addGr: {
                addToGroup(group, m_fields->m_objectsSource);
                break;
            }
            case sourceFuncType::addGrSM: {
                addToGroupSM(group, m_fields->m_objectsSource);
                break;
            }
            default: break;
        }

        // target object
        std::string modifiedTargetObject;
        if (!resetToDefault) {
            size_t pos = targetStr.find("g"); // group
            if (pos != std::string::npos) {
                targetStr.replace(pos, 1, std::to_string(group));
            }
            modifiedTargetObject = std::format("{},{}", m_fields->m_objectTargetCopy, targetStr);
        } else {
            modifiedTargetObject = m_fields->m_objectTargetCopy;
        }
        auto objArray = levelLayer->createObjectsFromString(modifiedTargetObject, true, true); 
        if (objArray->count() == 0) {
            log::debug("modified target object wasn't successfully created from string");
            showDebugText("Internal error");
            return;
        }
        auto newObj = static_cast<GameObject*>(objArray->objectAtIndex(0));
        newObj->setPosition(m_fields->m_objectTarget->getPosition());
        EditorUI::selectObject(m_fields->m_objectTarget, true);
        EditorUI::onDeleteSelected(nullptr);
        m_fields->m_objectTarget = newObj;

        levelLayer->m_undoObjects->removeLastObject();
        
        EditorUI::selectObject(newObj, true);
        EditorUI::updateButtons();
    }

    // ------------------------ utility ---------------------------

    srcObjType getObjectsCommonType(CCArray * objects) {
        auto type = srcObjType::trig;
        bool yes = true;
        for (unsigned i = 0; i < objects->count(); i++) {
            auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
            if (!triggerIDs.contains(obj->m_objectID)) {
                yes = false;
                break;
            }
        }
        if (yes) return type; 
        type = srcObjType::obj;
        return type;
    }

    std::vector<short> getObjectsCommonGroups(CCArray * objects) {
        if (objects->count() == 0) return {};
        auto firstObj = static_cast<GameObject*>(objects->objectAtIndex(0));

        if(!firstObj->m_groups) return std::vector<short>();

        std::set<short> commonGroups(firstObj->m_groups->begin(), firstObj->m_groups->end());
        commonGroups.erase(0);

        for (unsigned i = 1; i < objects->count(); i++) {
            auto groups = static_cast<GameObject*>(objects->objectAtIndex(i))->m_groups;
            if (!groups) return std::vector<short>();

            std::set<short> foundGroups;
            for (unsigned i = 0; i < groups->size(); i++) {
                short group = groups->at(i);
                if (group == 0) break;
                if (commonGroups.find(group) != commonGroups.end()) {
                    foundGroups.insert(group);
                }
            }
            commonGroups = foundGroups;
            if (commonGroups.size() == 0) {    
                break;
            }
        }
        return std::vector<short>(commonGroups.begin(), commonGroups.end());
    }

    bool isNewGroupPossible(CCArray * objects) {
        for (unsigned i = 0; i < objects->count(); i++) {
            auto groups = static_cast<GameObject*>(objects->objectAtIndex(i))->m_groups;
            if (!groups) continue;
            if (groups->at(9) != 0) {  
                return false;
            }
        }
        return true;
    }

    void addToGroup(int group, CCArray * objects) {
        for (unsigned i = 0; i < objects->count(); i++) {
            auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
            obj->addToGroup(group);
        }
    }

    void addToGroupSM(int group, CCArray * objects) {
        for (unsigned i = 0; i < objects->count(); i++) {
            auto obj = static_cast<EffectGameObject*>(objects->objectAtIndex(i));
            obj->addToGroup(group);
            obj->m_isSpawnTriggered = true;
            obj->m_isMultiTriggered = true;
        }
    }
    
    CCPoint calculateLineStartOnRectangle(CCPoint endPoint, std::pair<CCPoint, CCPoint> box) {
        auto lineStartY = endPoint.y;
        auto lineStartX = endPoint.x;
        short isPointInside = 0;
        if (lineStartX > box.second.x) lineStartX = box.second.x;
        else if (lineStartX < box.first.x) lineStartX = box.first.x;
        else isPointInside++;
        if (lineStartY > box.first.y) lineStartY = box.first.y;
        else if (lineStartY < box.second.y) lineStartY = box.second.y;
        else isPointInside++;
        if (isPointInside == 2) {
            return (box.first + box.second) / 2;
        }
        return ccp(lineStartX, lineStartY);
    }

    // -------------------- Converters for coordinates -------------------------

    CCPoint screenToEditorLayerPosition(CCPoint screenPoint) {
        auto glPoint = CCDirector::get()->convertToGL(screenPoint);
        auto objectLayer = LevelEditorLayer::get()->m_objectLayer;
        return objectLayer->convertToNodeSpace(this->convertToWorldSpace(glPoint));
    }

    // ------------------- updaters for interface -------------------------

    void updateLineAndRect(bool clear) {
        if (!m_fields->m_drawingLayer) {
            initDrawingLayer();
        }
        m_fields->m_drawingLayer->clear();
        if (!clear) {
            m_fields->m_drawingLayer->drawSegment(
                m_fields->m_lineStart,
                m_fields->m_lineEnd, 1.f,
                ccc4f(128, 128, 128, 155));
            m_fields->m_drawingLayer->drawRect(
                m_fields->m_sourceRectangle.first,
                m_fields->m_sourceRectangle.second,
                ccc4f(0, 0, 0, 0), 1.f, 
                ccc4f(128, 128, 128, 155));
        }
    }

    void controlTargetObjectCallback(float) { 
        static float endObjPosX = 0.f;
        static float endObjPosY = 0.f;
        auto endObj = m_fields->m_objectTarget;
        if (!endObj) return;
        if (!endObj->m_isSelected || this->getSelectedObjects()->count() > 1) {
            resetTool();
        } else if (endObj->getPositionX() != endObjPosX || endObj->getPositionY() != endObjPosY) {
            // if object was moved
            endObjPosX = endObj->getPositionX();
            endObjPosY = endObj->getPositionY();
            m_fields->m_lineEnd = endObj->getPosition();
            m_fields->m_lineStart = calculateLineStartOnRectangle(m_fields->m_lineEnd, m_fields->m_sourceRectangle);
            updateLineAndRect(false);
            if(m_fields->m_upperMenu) {
                m_fields->m_upperMenu->setPosition(endObj->getPosition() + ccp(5, 15));
            }
            if(m_fields->m_lowerMenu) {
                m_fields->m_lowerMenu->setPosition(endObj->getPosition() + ccp(5, -15));
            }
        }
    }

    void resetTool() {
        // reset menus
        if (m_fields->m_upperMenu) {
            m_fields->m_upperMenu->removeFromParent();
        }
        if (m_fields->m_lowerMenu) {
            m_fields->m_lowerMenu->removeFromParent();
        }
        // reset source objects
        if (m_fields->m_objectsSource) {
            auto selectedNow = EditorUI::getSelectedObjects();
            for (unsigned i = 0; i < m_fields->m_objectsSource->count(); i++) {
                auto obj = static_cast<GameObject*>(m_fields->m_objectsSource->objectAtIndex(i));
                EditorUI::deselectObject(obj);
            }
            EditorUI::selectObjects(selectedNow, true);
        }
        // reset line
        updateLineAndRect(true);
        // reset fields
        m_fields->m_upperMenu = nullptr;
        m_fields->m_lowerMenu = nullptr;
        m_fields->m_objectsSource = nullptr;
        m_fields->m_objectTarget = nullptr;
        m_fields->m_objectsSourceCopy = nullptr;
        m_fields->m_objectTargetCopy = "";
        m_fields->m_interfaceIsVisible = false;
        m_fields->m_globalConfig.isFinished = false;

        EditorUI::updateButtons();
        log::debug("tool reset");
    }
};

// ------------------------------- keybinds -----------------------------------

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
