#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <utility>
#include <sstream>
#include <optional>
#include <cstring>

#ifdef GEODE_IS_WINDOWS
#include <geode.custom-keybinds/include/Keybinds.hpp>
#endif

using namespace geode::prelude;

#include "configuration.hpp"

class $modify(MyEditorUI, EditorUI) {
    struct ToolConfig {
        Variant m_targetConfig;
        sourceFuncType m_sourceFunc;
        short m_group;
        bool m_isFinished = false;
    };

    struct ModSettings {
        bool m_previewColor = false;
        bool m_ctrlModifierEnabled = false;
    };

    enum Mode {
        SetupMode, PreviewMode
    };

    struct Fields {
        Mode m_mode = Mode::SetupMode;
        ModSettings m_modSettings;
        // for setup mode
        CCMenuItemSpriteExtra* m_button = nullptr;
        CCLabelBMFont* m_debugLabel = nullptr;
        CCDrawNode* m_drawingLayer = nullptr;
        CCPoint m_lineStart;  // both are editor coordinates
        CCPoint m_lineEnd;
        std::pair<CCPoint, CCPoint> m_sourceRectangle;
        Ref<CCArray> m_objectsSource = nullptr;  // array of selected objects
        Ref<GameObject> m_objectTarget = nullptr;  // end object
        Ref<CCArray> m_objectsSourceCopy = nullptr;
        std::string m_objectTargetInitial;
        std::string m_objectTargetLastUse;
        Ref<CCMenu> m_upperMenu = nullptr;
        Ref<CCMenu> m_lowerMenu = nullptr;
        bool m_buttonIsActivated = false;
        bool m_interfaceIsVisible = false;
        bool m_panEditor = false;
        ToolConfig m_globalConfig;
        // for preview mode
    };

    bool toolIsActivated() {
        #ifdef GEODE_IS_WINDOWS 
            return ((m_fields->m_modSettings.m_ctrlModifierEnabled && 
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
        // large button
        auto largeBtnSprite = CCSprite::createWithSpriteFrameName("TWT_tool_off.png"_spr);
        largeBtnSprite->setScale(.91f);
        auto largeBtn = CCMenuItemSpriteExtra::create(
            largeBtnSprite, this, menu_selector(MyEditorUI::onMainButton));
        m_fields->m_button = largeBtn;
        auto twtMenu = CCMenu::create();
        // info button
        if (!Mod::get()->getSettingValue<bool>("hide-info-button")) {
            auto smallBtn = CCMenuItemSpriteExtra::create(
                CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"), this,
                menu_selector(MyEditorUI::onInfoButton));
            twtMenu->addChild(smallBtn);
            smallBtn->setZOrder(2);
        }
        // mode selection buttons
        auto setupBtnSpr = ButtonSprite::create("Set up");
        setupBtnSpr->setScale(.5);
        auto setupBtn = CCMenuItemSpriteExtra::create(
			setupBtnSpr, this, menu_selector(MyEditorUI::onModeChanged));
        setupBtn->setTag(Mode::SetupMode);

        auto previewBtnSpr = ButtonSprite::create("Preview");
        previewBtnSpr->setScale(.5);
        auto previewBtn = CCMenuItemSpriteExtra::create(
			previewBtnSpr, this, menu_selector(MyEditorUI::onModeChanged));
        previewBtn->setTag(Mode::PreviewMode);

        // menu
        twtMenu->setContentWidth(largeBtn->getContentWidth() + previewBtn->getContentWidth());
        twtMenu->setContentHeight(largeBtn->getContentHeight());
        twtMenu->setID("twt-menu");
        
        twtMenu->addChild(largeBtn);
        largeBtn->setPosition(largeBtn->getContentSize() / 2);

        twtMenu->addChild(setupBtn);
        setupBtn->setPosition(ccp(largeBtn->getContentSize().width + setupBtn->getContentSize().width / 2, largeBtn->getContentHeight() * .75));

        twtMenu->addChild(previewBtn);
        previewBtn->setPosition(ccp(largeBtn->getContentSize().width + previewBtn->getContentSize().width / 2, largeBtn->getContentHeight() * .25));

        auto undoMenu = this->getChildByID("undo-menu");
        undoMenu->setContentSize(undoMenu->getContentSize() + ccp(50, 0));
        undoMenu->setPositionX(undoMenu->getPositionX() + 25);
        undoMenu->addChild(twtMenu);
        undoMenu->updateLayout();

        auto slider = this->getChildByID("position-slider");
        slider->setPositionX(slider->getPositionX() + 50);
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
                    this->onMainButton(nullptr);
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

    void onMainButton(CCObject* sender) {
        auto oldSprite = static_cast<CCMenuItemSpriteExtra*>(sender)->getNormalImage();
        m_fields->m_buttonIsActivated = !m_fields->m_buttonIsActivated;
        auto btn = m_fields->m_button;
        if (m_fields->m_buttonIsActivated) {
            auto sprite = CCSprite::createWithSpriteFrameName("TWT_tool_on.png"_spr);
            sprite->setScale(oldSprite->getScale());
            btn->setSprite(sprite);
            showDebugText("Tool is on");
        } else {
            auto sprite = CCSprite::createWithSpriteFrameName("TWT_tool_off.png"_spr);
            sprite->setScale(oldSprite->getScale());
            btn->setSprite(sprite);
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

    void onModeChanged(CCObject* sender) {
        Mode newMode = static_cast<Mode>(sender->getTag());
        if (m_fields->m_mode == newMode) return;
        m_fields->m_mode = newMode;
        resetTool();
        if (newMode == Mode::SetupMode) {
            showDebugText("Setup mode");
        } else {
            showDebugText("Preview mode");

        }
    }


    // --------------------------- overwritten functions ---------------------

    $override bool init(LevelEditorLayer * layer) {
        if (!EditorUI::init(layer)) return false;
        // init mod settings
        m_fields->m_modSettings.m_previewColor =  Mod::get()->getSettingValue<bool>("preview-color");
        #ifdef GEODE_IS_WINDOWS
            m_fields->m_modSettings.m_ctrlModifierEnabled = Mod::get()->getSettingValue<bool>("control-key-modifier");
        #endif
        initButtons();
        setKeybinds();
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
        m_fields->m_globalConfig.m_targetConfig = config;

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
        m_fields->m_globalConfig.m_group = config.second;

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
        m_fields->m_objectTargetInitial = targetObj->getSaveString(nullptr);
        m_fields->m_objectTargetLastUse = m_fields->m_objectTargetInitial;
        
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
                // todo: optional "smart filter" for this situation
                log::debug("this combination is not supported");
                return;
            }
        }
        m_fields->m_globalConfig.m_sourceFunc = forSourceObjType->second.m_srcFuncType;

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
            switch (m_fields->m_globalConfig.m_sourceFunc) {
                case sourceFuncType::addGr:
                case sourceFuncType::addGrSM:
                case sourceFuncType::addGrAnim: {
                    auto newGroupPossible = isNewGroupPossible(m_fields->m_objectsSource);
                    if (newGroupPossible) {
                        int nextFree = levelLayer->getNextFreeGroupID(nullptr);
                        lowerMenuConfig.push_back({std::format("next ({})", nextFree), nextFree});
                        // lowerMenuConfig.push_back({"next free", nextFree});
                    }
                    auto commonGroups = getObjectsCommonGroups(m_fields->m_objectsSource);
                    for (unsigned i = 0; i < commonGroups.size(); i++) {
                        lowerMenuConfig.push_back({std::format("group {}", commonGroups.at(i)), commonGroups.at(i)});
                    }
                    lowerMenuConfig.push_back({"None", -1});
                    break;
                }
                case sourceFuncType::color: {
                    auto allColors = getObjectsAllColors(m_fields->m_objectsSource);
                    auto maybeCommonBaseCol = getCommonBaseColor(m_fields->m_objectsSource);
                    auto maybeCommonDetailCol = getCommonDetailColor(m_fields->m_objectsSource);
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

        m_fields->m_globalConfig.m_isFinished = true;
        applyToolConfig();
    }


    void applyToolConfig() {
        if (!m_fields->m_globalConfig.m_isFinished) return;
        auto targetStr = m_fields->m_globalConfig.m_targetConfig.m_triggerConfigString;
        auto conditionalTargetStr = m_fields->m_globalConfig.m_targetConfig.m_triggerConditionalConfigString;
        auto sourceFunc = m_fields->m_globalConfig.m_sourceFunc;
        short group = m_fields->m_globalConfig.m_group;
        bool resetToDefault = group == -1;
        auto levelLayer = LevelEditorLayer::get();

        { // check if target object was changed after previous tool use
            auto targetObjNow = m_fields->m_objectTarget->getSaveString(nullptr);
            auto targetObjLast = m_fields->m_objectTargetLastUse;
            log::debug("previous initial: {}", m_fields->m_objectTargetInitial);
            m_fields->m_objectTargetInitial = applyDifference(
                targetObjLast, targetObjNow, m_fields->m_objectTargetInitial);
            log::debug("current initial: {}", m_fields->m_objectTargetInitial);
        }
        
        // source objects
        for (unsigned i = 0; i < m_fields->m_objectsSource->count(); i++) {
            auto obj = static_cast<GameObject*>(m_fields->m_objectsSource->objectAtIndex(i));
            auto objDefaultAttributes = static_cast<GameObject*>(m_fields->m_objectsSourceCopy->objectAtIndex(i));
            myCopyGroups(objDefaultAttributes, obj); // restore initial groups
        }

        if (!resetToDefault) switch (m_fields->m_globalConfig.m_sourceFunc) {
            case sourceFuncType::addGr: {
                addToGroup(group, m_fields->m_objectsSource);
                break;
            }
            case sourceFuncType::addGrSM: {
                // todo: optionally disable this
                addToGroupSM(group, m_fields->m_objectsSource);
                break;
            }
            case sourceFuncType::addGrAnim: {
                // and this
                addToGroupAnim(group, m_fields->m_objectsSource);
                break;
            }
            case sourceFuncType::color: {
                // deselect source objects to be able to preview color
                if (m_fields->m_modSettings.m_previewColor) {
                    bool wereSelected = false;
                    for (unsigned i = 0; i < m_fields->m_objectsSource->count(); i++) {
                        auto obj = static_cast<GameObject*>(m_fields->m_objectsSource->objectAtIndex(i));
                        if (obj->m_isSelected) wereSelected = true;
                        EditorUI::deselectObject(obj);
                    }
                    if (wereSelected) showDebugText("Color preview", 1);
                }
                break;
            }
            default: break;
        }

        // target object
        std::string modifiedTargetObject;
        std::string initialTargetObject = m_fields->m_objectTargetInitial;
        if (!resetToDefault) {
            size_t pos = targetStr.find("g"); // g - group
            if (pos != std::string::npos) {
                targetStr.replace(pos, 1, std::to_string(group));
            }
            modifiedTargetObject = std::format("{},{}", initialTargetObject, targetStr);
            // conditional config
            auto objKeyVal = objectToKeyVal(initialTargetObject);
            for (unsigned i = 0; i < conditionalTargetStr.size(); i++) {
                auto condition = conditionalTargetStr.at(i);
                if (objKeyVal.contains(condition.m_condition.first) && 
                    (objKeyVal[condition.m_condition.first] == "" || 
                    objKeyVal[condition.m_condition.first] == condition.m_condition.second)) {
                    // if key and value exist, add conditional "yes" string
                    modifiedTargetObject = std::format("{},{}", modifiedTargetObject, condition.m_yes);
                    log::debug("conditional YES");
                } else {
                    // if key or value not exist, add conditional "no" string
                    modifiedTargetObject = std::format("{},{}", modifiedTargetObject, condition.m_no);
                    log::debug("conditional NO");
                }
            }
        } else {
            modifiedTargetObject = initialTargetObject;
        }
        auto objArray = levelLayer->createObjectsFromString(modifiedTargetObject, true, true); 
        if (objArray->count() == 0) {
            log::debug("modified target object wasn't successfully created from string");
            showDebugText("Internal error");
            return;
        }
        auto newObj = static_cast<GameObject*>(objArray->objectAtIndex(0));
        EditorUI::selectObject(m_fields->m_objectTarget, true);
        EditorUI::onDeleteSelected(nullptr);
        m_fields->m_objectTarget = newObj;

        levelLayer->m_undoObjects->removeLastObject();
        
        m_fields->m_objectTargetLastUse = newObj->getSaveString(nullptr);
        EditorUI::selectObject(newObj, true);
        EditorUI::updateButtons();
    }

    // ------------------------ utility ---------------------------

    srcObjType getObjectsCommonType(CCArray * objects) {
        // trig
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
        // anim
        type = srcObjType::anim;
        yes = true;
        for (unsigned i = 0; i < objects->count(); i++) {
            auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
            if (!animatedIDs.contains(obj->m_objectID)) {
                yes = false;
                break;
            }
        }
        if (yes) return type;
        // keyframe
        type = srcObjType::anim;
        yes = true;
        for (unsigned i = 0; i < objects->count(); i++) {
            auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
            if (obj->m_objectID == keyFrameOjb) {
                yes = false;
                break;
            }
        }
        if (yes) return type;
        // else
        return srcObjType::any;
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

    void addToGroupAnim(int group, CCArray * objects) {
        for (unsigned i = 0; i < objects->count(); i++) {
            auto obj = static_cast<EffectGameObject*>(objects->objectAtIndex(i));
            obj->addToGroup(group);
            obj->m_animateOnTrigger = true;
        }
    }

    void myCopyGroups(GameObject* from, GameObject* to) {
        if (!to->m_groups) to->addToGroup(1);
        if (from->m_groups) {
            std::memcpy(to->m_groups, from->m_groups, sizeof(short) * 10);
        } else {
            to->m_groups->fill(0);
        }
        to->m_groupCount = from->m_groupCount;
    }

    std::vector<int> getObjectsAllColors(CCArray * objects) {
        std::set<int> colorIds;
        for (unsigned i = 0; i < objects->count(); i++) {
            auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
            if (obj->m_baseColor) {
                auto colorId = obj->m_baseColor->m_colorID;
                if (colorId == 0) colorId = obj->m_baseColor->m_defaultColorID;
                colorIds.insert(colorId);
            }
            if (obj->m_detailColor) {
                auto colorId = obj->m_detailColor->m_colorID;
                if (colorId == 0) colorId = obj->m_detailColor->m_defaultColorID;
                colorIds.insert(colorId);
            }
        }
        return std::vector<int>(colorIds.begin(), colorIds.end());
    }

    std::optional<int> getCommonBaseColor(CCArray * objects) {
        int commonBaseCol = -1;
        for (unsigned i = 0; i < objects->count(); i++) {
            auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
            if (obj->m_baseColor) {
                auto colorId = obj->m_baseColor->m_colorID;
                if (colorId == 0) colorId = obj->m_baseColor->m_defaultColorID;
                if (commonBaseCol != colorId) {
                    if (commonBaseCol == -1) commonBaseCol = colorId;
                    else return {};
                }
            }
        }
        if (commonBaseCol == -1) return {};
        return commonBaseCol;
    }

    std::optional<int> getCommonDetailColor(CCArray * objects) {
        int commonDetailCol = -1;
        for (unsigned i = 0; i < objects->count(); i++) {
            auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
            if (obj->m_detailColor) {
                auto colorId = obj->m_detailColor->m_colorID;
                if (colorId == 0) colorId = obj->m_detailColor->m_defaultColorID;
                if (commonDetailCol != colorId) {
                    if (commonDetailCol == -1) commonDetailCol = colorId;
                    else return {};
                }
            }
        }
        if (commonDetailCol == -1) return {};
        return commonDetailCol;
    }

    std::map<std::string, std::string> objectToKeyVal(std::string objSaveString) {
        std::stringstream ss(objSaveString);
        std::map<std::string, std::string> keyVals;
        std::string key, val;
        while (std::getline(ss, key, ',') && std::getline(ss, val, ',')) {
            keyVals.insert({key, val});
        }
        return keyVals;
    }

    // needed to check if something was changed in targetObj via editObject menu or somehow else
    // while tool interface was open. This function computes this difference and applies
    // it to the "obj" witch is an initial state of targetObj
    std::string applyDifference(std::string before, std::string after, std::string obj) {
        auto kvBefore = objectToKeyVal(before);
        auto kvAfter = objectToKeyVal(after);
        auto kvObj = objectToKeyVal(obj);
        // remove not changed elements
        for (auto it = kvBefore.begin(); it != kvBefore.end();) {
            if (kvAfter.contains(it->first) && it->second == kvAfter.at(it->first)) {
                kvAfter.erase(it->first);
                it = kvBefore.erase(it);
            } else it++;
        }
        // remove kv-s from "before"
        for (auto it = kvBefore.begin(); it != kvBefore.end(); it++) {
            kvObj.erase(it->first);
        }
        // and add from after
        for (auto it = kvAfter.begin(); it != kvAfter.end(); it++) {
            kvObj.insert(*it);
        }
        // convert key-value map to obj string
        std::string result;
        for (auto it = kvObj.begin(); it != kvObj.end(); it++) {
            result += std::format("{},{},", it->first, it->second);
        }
        result.pop_back();
        return result;
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
        m_fields->m_objectTargetInitial = "";
        m_fields->m_interfaceIsVisible = false;
        m_fields->m_globalConfig.m_isFinished = false;

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
