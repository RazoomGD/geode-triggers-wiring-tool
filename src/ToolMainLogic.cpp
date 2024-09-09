#include "EditorUI.hpp"

bool MyEditorUI::toolIsActivated() {
    #ifdef GEODE_IS_WINDOWS 
        return ((m_fields->m_modSettings.m_ctrlModifierEnabled && 
            CCKeyboardDispatcher::get()->getControlKeyPressed()) || 
            m_fields->m_buttonIsActivated) && !m_fields->m_panEditor;
    #else
        return m_fields->m_buttonIsActivated;
    #endif
}

// this is called only once before each tool use 
// and meant to set initial configuration and prepare everything
bool MyEditorUI::handleTargetObject() {
    if (!m_fields->m_objectTarget || !m_fields->m_objectsSource) return false;
    auto levelLayer = LevelEditorLayer::get();
    auto targetObj = m_fields->m_objectTarget;
    EditorUI::selectObject(targetObj, true);
    if (EditorUI::getSelectedObjects()->count() > 1) {
        // this may happen if target object has linked objects
        showDebugText("Multiple target objects\naren't supported yet", 1.f);
        return false;
    }
    // create and save copies of target object 
    m_fields->m_objectTargetInitial = targetObj->getSaveString(nullptr);
    m_fields->m_objectTargetLastUse = m_fields->m_objectTargetInitial;
    // set next free group
    m_fields->m_globalConfig.m_nextFreeGroup = LevelEditorLayer::get()->getNextFreeGroupID(nullptr);
    
    // create and save copy of source objects
    m_fields->m_objectsSourceCopy = CCArray::create();
    for (unsigned i = 0; i < m_fields->m_objectsSource->count(); i++) {
        auto obj = static_cast<GameObject*>(m_fields->m_objectsSource->objectAtIndex(i));
        auto objCopy = new TWTObjCopy();
        myCopyObjectProps(obj, objCopy);
        m_fields->m_objectsSourceCopy->addObject(objCopy);
    }

    // get configuration for given source and target objects
    if (!CONFIGURATION.contains(targetObj->m_objectID)) {
        // we don't have any options for given object (probably because target is not a trigger)
        // todo: common config (just copy groups from target to source)
        log::debug("obj not found in map");
        return false;
    } 
    // find source objects common types
    std::set<objId> srcObjIds;
    for (unsigned i = 0; i < m_fields->m_objectsSource->count(); i++) {
        auto obj = static_cast<GameObject*>(m_fields->m_objectsSource->objectAtIndex(i));
        srcObjIds.insert(obj->m_objectID);
    }
    std::set<srcObjType> commonTypes = getTypesById(*(srcObjIds.begin()));
    for (auto objId : srcObjIds) {
        auto objTypes = getTypesById(objId);
        for (auto it = commonTypes.begin(); it != commonTypes.end();) {
            if (!objTypes.contains(*it)) {
                it = commonTypes.erase(it);
            } else it++;
        }
    }
    auto targetConfig = CONFIGURATION.find(targetObj->m_objectID)->second;
    // get variants for upper menu
    std::vector<Variant> upperMenuVariants;
    if (m_fields->m_modSettings.m_smartFilter) {
        std::vector<Variant> anyOnlyUpperMenuVariants;
        for (auto &variant : targetConfig) {
            // when common type is not only any we (by default) dont show "any-only" variants
            // we show them only when no other variants available
            if (commonTypes.size() > 1 && variant.m_srcObjType == std::set{srcObjType::any}) {
                anyOnlyUpperMenuVariants.push_back(variant);
                continue;
            }
            
            // if at least one object matches the variant, we accept this variant
            for (auto objId : srcObjIds) {
                auto objTypes = getTypesById(objId);
                // if (objTypes.size() > 1) objTypes.erase(srcObjType::any);
                bool acceptVariant = true;
                for (auto requiredType : variant.m_srcObjType) {
                    if (!objTypes.contains(requiredType)) {
                        acceptVariant = false;
                        break;
                    }
                }
                if (acceptVariant) {
                    upperMenuVariants.push_back(variant);
                    break;
                }
            }  
        }
        if (!upperMenuVariants.size()) {
            upperMenuVariants = anyOnlyUpperMenuVariants;
        }

        // for (auto &type : srcTypes) {
        //     if (targetConfig.contains(type)) {
        //         auto targetSourceConfig = targetConfig.find(type)->second;
        //         upperMenuVariants.insert(upperMenuVariants.end(), 
        //             targetSourceConfig.begin(), targetSourceConfig.end());
        //     }
        // }
        // if (!upperMenuVariants.size() && targetConfig.contains(srcObjType::any)) {
        //     upperMenuVariants = targetConfig.find(srcObjType::any)->second;
        // }
    } else {
        for (auto &variant : targetConfig) { // todo: not tested yet
            bool acceptVariant = true;
            for (auto requiredType : variant.m_srcObjType) {
                if (!commonTypes.contains(requiredType)) {
                    acceptVariant = false;
                    break;
                }
            }
            if (acceptVariant) {
                upperMenuVariants.push_back(variant);
            }
        }

        // bool haveCommonType = srcTypes.size() == 1;
        // auto commonSrcType = haveCommonType ? *(srcTypes.begin()) : srcObjType::any;
        // if (targetConfig.contains(commonSrcType)) {
        //     upperMenuVariants = targetConfig.find(commonSrcType)->second;
        // } else if (commonSrcType != srcObjType::any && targetConfig.contains(srcObjType::any)) {
        //     upperMenuVariants = targetConfig.find(srcObjType::any)->second;
        // } else {
        //     log::debug("this combination is not supported");
        //     return false;
        // } 
    }

    { // create upper menu
        if (!upperMenuVariants.size()) {
            log::debug("this combination is not supported");
            resetTool();
            return false;
        }
        createUpperMenu(upperMenuVariants, true, commonTypes);
        auto upperMenu = m_fields->m_upperMenu;
        m_fields->m_drawingLayer->addChild(upperMenu);
        upperMenu->setPosition(targetObj->getPosition() + ccp(5, 15));
    }
    return true;
}

void MyEditorUI::applyToolConfig() {
    if (!m_fields->m_globalConfig.m_isFinished) return;
    auto targetStr = m_fields->m_globalConfig.m_variant.m_triggerConfigString;
    auto conditionalTargetStr = m_fields->m_globalConfig.m_variant.m_triggerConditionalConfigString;
    auto sourceFunc = m_fields->m_globalConfig.m_variant.m_srcFuncType;
    auto group = m_fields->m_globalConfig.m_group;
    bool resetToDefault = (group == -1);
    auto levelLayer = LevelEditorLayer::get();

    { // check if target object was changed after previous tool use
        auto targetObjNow = m_fields->m_objectTarget->getSaveString(nullptr);
        auto targetObjLast = m_fields->m_objectTargetLastUse;
        m_fields->m_objectTargetInitial = applyDifference(
            targetObjLast, targetObjNow, m_fields->m_objectTargetInitial);
    }
    
    // source objects
    for (unsigned i = 0; i < m_fields->m_objectsSource->count(); i++) {
        auto obj = static_cast<GameObject*>(m_fields->m_objectsSource->objectAtIndex(i));
        auto objDefaultAttributes = static_cast<TWTObjCopy*>(m_fields->m_objectsSourceCopy->objectAtIndex(i));
        myPasteObjectProps(objDefaultAttributes, obj); // restore initial groups
    }

    auto objectsSource = m_fields->m_objectsSourceFiltered ? m_fields->m_objectsSourceFiltered : m_fields->m_objectsSource;
    if (!resetToDefault) switch (m_fields->m_globalConfig.m_variant.m_srcFuncType) {
        case sourceFuncType::addGr: {
            addToGroup(group, objectsSource);
            break;
        }
        case sourceFuncType::addGrSM: {
            // todo: optionally disable this
            addToGroupSM(group, objectsSource);
            break;
        }
        case sourceFuncType::addGrAnim: {
            // and this
            addToGroupAnim(group, objectsSource);
            break;
        }
        case sourceFuncType::addGrCol: 
            addToGroup(group, objectsSource);
            // no break
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
        while (true) {
            size_t pos = targetStr.find("g"); // g - group
            if (pos != std::string::npos) {
                targetStr.replace(pos, 1, std::to_string(group));
            } else break;
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

