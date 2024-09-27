#include "EditLogic.hpp"
#include "EditToolConfig.hpp"
#include "../EditorUI.hpp"
#include "Utils.hpp"


bool EditLogic::toolIsActivated() {
    #ifdef GEODE_IS_WINDOWS 
        return ((m_editorInstance->m_fields->m_modSettings.m_ctrlModifierEnabled && 
            CCKeyboardDispatcher::get()->getControlKeyPressed()) || 
            m_buttonIsActivated) && !m_panEditor;
    #else
        return m_buttonIsActivated;
    #endif
}

// this is called only once before each tool use 
// and meant to set initial configuration and prepare everything
bool EditLogic::handleTargetObject() {
    if (!m_objectTarget || !m_objectsSource) return false;
    auto levelLayer = LevelEditorLayer::get();
    auto targetObj = m_objectTarget;
    m_editorInstance->selectObject(targetObj, true);
    if (m_editorInstance->getSelectedObjects()->count() > 1) {
        // this may happen if target object has linked objects
        m_editorInstance->showDebugText("Multiple target objects\naren't supported yet", 1.f);
        return false;
    } else if (m_editorInstance->getSelectedObjects()->count() < 1) {
        // this may happen when select filter is enabled
        m_editorInstance->showDebugText("No objects selected");
        return false;
    }
    // create and save copies of target object 
    m_objectTargetInitial = targetObj->getSaveString(nullptr);
    m_objectTargetLastUse = m_objectTargetInitial;
    m_objectTargetTWTCopy = new TWTObjCopy(); 
    myCopyObjectProps(m_objectTarget, m_objectTargetTWTCopy);
    // set next free group
    m_globalConfig.m_nextFreeGroup = levelLayer->getNextFreeGroupID(nullptr);
    
    // create and save copy of source objects
    m_objectsSourceCopy = CCArray::create();
    for (unsigned i = 0; i < m_objectsSource->count(); i++) {
        auto obj = static_cast<GameObject*>(m_objectsSource->objectAtIndex(i));
        auto objCopy = new TWTObjCopy();
        myCopyObjectProps(obj, objCopy);
        m_objectsSourceCopy->addObject(objCopy);
    }

    // get configuration for given source and target objects
    if (!CONFIGURATION.contains(targetObj->m_objectID)) {
        // we don't have any options for given object (probably because target is not a trigger)
        log::debug("obj not found in map");
        if (m_editorInstance->m_fields->m_modSettings.m_defaultIsCopyGroup) {
            auto variants = std::vector<Variant>{copyGroupDefaultVariant};
            auto types = std::set<srcObjType>{srcObjType::any};
            createUpperMenu(variants, true, types);
            m_editorInstance->updateButtons();
            return true;
        } else {
            resetTool();
            return false;
        }
    } 
    // find source objects common types
    std::set<objId> srcObjIds;
    for (unsigned i = 0; i < m_objectsSource->count(); i++) {
        auto obj = static_cast<GameObject*>(m_objectsSource->objectAtIndex(i));
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
    std::vector<Variant> anyUpperMenuVariants;
    if (m_editorInstance->m_fields->m_modSettings.m_smartFilter) {
        for (auto &variant : targetConfig) {
            // when common type is not only any we (by default) dont show "any-only" variants
            // we show them only when no other variants available
            if (commonTypes.size() > 1 && variant.m_srcObjType == srcObjType::any) {
                anyUpperMenuVariants.push_back(variant);
                continue;
            }
            // if at least one object matches the variant, we accept this variant
            for (auto objId : srcObjIds) {
                auto objTypes = getTypesById(objId);
                if (objTypes.contains(variant.m_srcObjType)) {
                    upperMenuVariants.push_back(variant);
                    break;
                }
            }  
        }
    } else {
        for (auto &variant : targetConfig) {
            if (commonTypes.size() > 1 && variant.m_srcObjType == srcObjType::any) {
                anyUpperMenuVariants.push_back(variant);
                continue;
            }
            if (commonTypes.contains(variant.m_srcObjType)) {
                upperMenuVariants.push_back(variant);
            }
        }
    }

    if (!upperMenuVariants.size()) {
        upperMenuVariants = anyUpperMenuVariants;
    }

    { // create upper menu
        if (!upperMenuVariants.size()) {
            log::debug("this combination is not supported");
            resetTool();
            return false;
        }
        createUpperMenu(upperMenuVariants, true, commonTypes);
    }
    return true;
}

void EditLogic::applyToolConfig(bool updateSourceObjects) {
    if (!m_globalConfig.m_isFinished) return;
    auto targetStr = m_globalConfig.m_variant.m_triggerConfigString;
    const auto& conditionalTargetStr = m_globalConfig.m_variant.m_triggerConditionalConfigString;
    const auto sourceFunc = m_globalConfig.m_variant.m_srcFuncType;
    const auto group = m_globalConfig.m_group;
    const bool resetToDefault = (group == -1);
    const auto levelLayer = LevelEditorLayer::get();

    // source objects
    if (updateSourceObjects) {
        for (unsigned i = 0; i < m_objectsSource->count(); i++) {
            auto obj = static_cast<GameObject*>(m_objectsSource->objectAtIndex(i));
            auto objDefaultAttributes = static_cast<TWTObjCopy*>(m_objectsSourceCopy->objectAtIndex(i));
            myPasteObjectProps(objDefaultAttributes, obj); // restore initial groups
        }

        auto objectsSource = m_objectsSourceFiltered;
        if (!resetToDefault) switch (m_globalConfig.m_variant.m_srcFuncType) {
            case sourceFuncType::addGr: {
                addToGroup(group, objectsSource);
                break;
            }
            case sourceFuncType::addGrSM: {
                addToGroupSM(group, objectsSource);
                break;
            }
            case sourceFuncType::addGrAnim: {
                addToGroupAnim(group, objectsSource);
                break;
            }
            case sourceFuncType::addGrCol: 
                addToGroup(group, objectsSource);
                // no break
            case sourceFuncType::color: {
                // deselect source objects to be able to preview color
                if (m_editorInstance->m_fields->m_modSettings.m_previewColor) {
                    bool wereSelected = false;
                    for (unsigned i = 0; i < m_objectsSource->count(); i++) {
                        auto obj = static_cast<GameObject*>(m_objectsSource->objectAtIndex(i));
                        if (obj->m_isSelected) wereSelected = true;
                        m_editorInstance->deselectObject(obj);
                    }
                    if (wereSelected) m_editorInstance->showDebugText("Color preview", 1);
                }
                break;
            }
            case sourceFuncType::setItem: {
                setItemId(group, objectsSource);
                break;
            }
            case sourceFuncType::setCollision: {
                setCollisionId(group, objectsSource);
                break;
            }
            case sourceFuncType::honestAddGr: {
                bool done = m_editorInstance->honestAddToGroup(group, objectsSource);
                if (!done) addToGroup(group, objectsSource);
                break;
            }
            case sourceFuncType::copyGroup: {
                addToGroup(group, objectsSource);
                break;
            }
            default: break;
        }
    }

    if (sourceFunc == sourceFuncType::copyGroup) {
        // on this option we don't need to change target object
        return;
    }

    { // check if target object was changed after previous tool use
        auto targetObjNow = m_objectTarget->getSaveString(nullptr);
        auto targetObjLast = m_objectTargetLastUse;
        m_objectTargetInitial = applyDifference(
            targetObjLast, targetObjNow, m_objectTargetInitial);
    }

    // target object
    std::string modifiedTargetObject;
    std::string initialTargetObject = m_objectTargetInitial;
    if (!resetToDefault) {
        while (true) {
            size_t pos = targetStr.find("g"); // g - group
            if (pos != std::string::npos) {
                targetStr.replace(pos, 1, std::to_string(group));
            } else break;
        }
        if (!m_globalConfig.m_variant.m_appendNotOverride) {
            modifiedTargetObject = std::format("{},{}", initialTargetObject, targetStr);
        } else {
            auto kvConfigStr = objectToKeyVal(targetStr);
            auto kvObject = objectToKeyVal(initialTargetObject);
            modifiedTargetObject = "1,1";
            for (auto kv : kvObject) {
                auto key = kv.first;
                auto val = kv.second + (kvConfigStr.contains(key) ? "." + kvConfigStr[key] : "");
                kvConfigStr.erase(key);
                modifiedTargetObject = std::format("{},{},{}", modifiedTargetObject, key, val);
            }
            for (auto kv: kvConfigStr) {
                modifiedTargetObject = std::format("{},{},{}", modifiedTargetObject, kv.first, kv.second);
            }
        }
        // conditional config
        auto objKeyVal = objectToKeyVal(initialTargetObject);
        for (unsigned i = 0; i < conditionalTargetStr.size(); i++) {
            auto condition = conditionalTargetStr.at(i);
            if (objKeyVal.contains(condition.m_condition.first) && 
                (objKeyVal[condition.m_condition.first] == "" || 
                objKeyVal[condition.m_condition.first] == condition.m_condition.second)) {
                // if key and value exist, add conditional "yes" string
                modifiedTargetObject = std::format("{},{}", modifiedTargetObject, condition.m_yes);
            } else {
                // if key or value not exist, add conditional "no" string
                modifiedTargetObject = std::format("{},{}", modifiedTargetObject, condition.m_no);
            }
        }
    } else {
        modifiedTargetObject = initialTargetObject;
    }
    auto objArray = levelLayer->createObjectsFromString(modifiedTargetObject, true, true); 
    if (objArray->count() == 0) {
        log::error("modified target object wasn't successfully created from string");
        m_editorInstance->showDebugText("Internal error");
        return;
    }
    auto newObj = static_cast<GameObject*>(objArray->objectAtIndex(0));

    // if an old target object was in source objects array, new target object should 
    // be added to m_objectsSource array, m_objectsSourceCopy array and 
    // m_objectsSourceFiltered array
    if (m_objectsSource->containsObject(m_objectTarget)) {
        log::debug("target obj contains in source array");
        // add to source
        m_objectsSource->addObject(newObj);
        // add copy to sourceCopy
        m_objectsSourceCopy->addObject(m_objectTargetTWTCopy);
        // add to filtered objects maybe
        if (m_objectsSourceFiltered->containsObject(m_objectTarget)) {
            m_objectsSourceFiltered->addObject(newObj);
        }
    }

    m_editorInstance->selectObject(m_objectTarget, true);
    m_editorInstance->onDeleteSelected(nullptr);
    m_objectTarget = newObj;

    { // fix undo-redo behaviour
        auto newUndo = UndoObject::createWithArray(objArray, UndoCommand::Paste);
        auto& undo = m_editorInstance->m_fields->m_undoTwiceObjUIDs;
        auto& redo = m_editorInstance->m_fields->m_redoTwiceObjUIDs;
        undo.push_front(newUndo->m_uID);
        redo.push_front(levelLayer->m_undoObjects->lastObject()->m_uID);
        levelLayer->m_undoObjects->addObject(newUndo);
        auto maxUndoRedo = levelLayer->m_increaseMaxUndoRedo ? 1000 : 200;
        if (undo.size() > maxUndoRedo) undo.pop_back();
        if (redo.size() > maxUndoRedo) redo.pop_back();
    }
    
    m_objectTargetLastUse = newObj->getSaveString(nullptr);
    m_editorInstance->selectObject(newObj, true);
    m_editorInstance->updateButtons(); 

    static int applyCount = 0;
    log::debug("apply tool config {}, group={}", ++applyCount, group);
}

