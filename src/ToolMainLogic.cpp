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
    auto targetConfig = CONFIGURATION.find(targetObj->m_objectID);
    if (targetConfig == CONFIGURATION.end()) {
        // we don't have any options for given object (probably because target is not a trigger)
        // todo: common config (just copy groups from target to source)
        log::debug("obj not found in map");
        return false;
    } 
    // (else) we have config for this object 
    // get variants for upper menu
    std::vector<Variant> upperMenuVariants;
    auto srcTypes = getObjectsAllTypes(m_fields->m_objectsSource);
    if (m_fields->m_modSettings.m_smartFilter) {
        srcTypes.insert(srcObjType::any);
        for (auto &type : srcTypes) {
            if (targetConfig->second.contains(type)) {
                auto targetSourceConfig = targetConfig->second.find(type)->second;
                upperMenuVariants.insert(upperMenuVariants.end(), 
                    targetSourceConfig.begin(), targetSourceConfig.end());
            }
        }
    } else {
        bool haveCommonType = srcTypes.size() == 1;
        auto type = haveCommonType ? *(srcTypes.begin()) : srcObjType::any;
        if (targetConfig->second.contains(type)) {
            upperMenuVariants = targetConfig->second.find(type)->second;
        } else if (targetConfig->second.contains(srcObjType::any)) {
            upperMenuVariants = targetConfig->second.find(srcObjType::any)->second;
        } else {
            log::debug("this combination is not supported");
            return false;
        } 
    }

    { // create upper menu
        if (upperMenuVariants.size() == 0) {
            resetTool();
            return false;
        }
        // todo: как то сделать чтобы не общие типы при включенном фильтре помечались звездочкой
        createUpperMenu(upperMenuVariants, true);
        auto upperMenu = m_fields->m_upperMenu;
        m_fields->m_drawingLayer->addChild(upperMenu);
        upperMenu->setPosition(targetObj->getPosition() + ccp(5, 15));
    }
    return true;
}


void MyEditorUI::applyToolConfig() {
    // todo: tmp

    // if (!m_fields->m_globalConfig.m_isFinished) return;
    // auto targetStr = m_fields->m_globalConfig.m_targetConfig.m_triggerConfigString;
    // auto conditionalTargetStr = m_fields->m_globalConfig.m_targetConfig.m_triggerConditionalConfigString;
    // auto sourceFunc = m_fields->m_globalConfig.m_sourceFuncType;
    // short group = m_fields->m_globalConfig.m_group;
    // bool resetToDefault = group == -1;
    // auto levelLayer = LevelEditorLayer::get();

    // { // check if target object was changed after previous tool use
    //     auto targetObjNow = m_fields->m_objectTarget->getSaveString(nullptr);
    //     auto targetObjLast = m_fields->m_objectTargetLastUse;
    //     log::debug("previous initial: {}", m_fields->m_objectTargetInitial);
    //     m_fields->m_objectTargetInitial = applyDifference(
    //         targetObjLast, targetObjNow, m_fields->m_objectTargetInitial);
    //     log::debug("current initial: {}", m_fields->m_objectTargetInitial);
    // }
    
    // // source objects
    // for (unsigned i = 0; i < m_fields->m_objectsSource->count(); i++) {
    //     auto obj = static_cast<GameObject*>(m_fields->m_objectsSource->objectAtIndex(i));
    //     auto objDefaultAttributes = static_cast<GameObject*>(m_fields->m_objectsSourceCopy->objectAtIndex(i));
    //     myCopyGroups(objDefaultAttributes, obj); // restore initial groups
    // }

    // if (!resetToDefault) switch (m_fields->m_globalConfig.m_sourceFuncType) {
    //     case sourceFuncType::addGr: {
    //         addToGroup(group, m_fields->m_objectsSource);
    //         break;
    //     }
    //     case sourceFuncType::addGrSM: {
    //         // todo: optionally disable this
    //         addToGroupSM(group, m_fields->m_objectsSource);
    //         break;
    //     }
    //     case sourceFuncType::addGrAnim: {
    //         // and this
    //         addToGroupAnim(group, m_fields->m_objectsSource);
    //         break;
    //     }
    //     case sourceFuncType::color: {
    //         // deselect source objects to be able to preview color
    //         if (m_fields->m_modSettings.m_previewColor) {
    //             bool wereSelected = false;
    //             for (unsigned i = 0; i < m_fields->m_objectsSource->count(); i++) {
    //                 auto obj = static_cast<GameObject*>(m_fields->m_objectsSource->objectAtIndex(i));
    //                 if (obj->m_isSelected) wereSelected = true;
    //                 EditorUI::deselectObject(obj);
    //             }
    //             if (wereSelected) showDebugText("Color preview", 1);
    //         }
    //         break;
    //     }
    //     default: break;
    // }

    // // target object
    // std::string modifiedTargetObject;
    // std::string initialTargetObject = m_fields->m_objectTargetInitial;
    // if (!resetToDefault) {
    //     size_t pos = targetStr.find("g"); // g - group
    //     if (pos != std::string::npos) {
    //         targetStr.replace(pos, 1, std::to_string(group));
    //     }
    //     modifiedTargetObject = std::format("{},{}", initialTargetObject, targetStr);
    //     // conditional config
    //     auto objKeyVal = objectToKeyVal(initialTargetObject);
    //     for (unsigned i = 0; i < conditionalTargetStr.size(); i++) {
    //         auto condition = conditionalTargetStr.at(i);
    //         if (objKeyVal.contains(condition.m_condition.first) && 
    //             (objKeyVal[condition.m_condition.first] == "" || 
    //             objKeyVal[condition.m_condition.first] == condition.m_condition.second)) {
    //             // if key and value exist, add conditional "yes" string
    //             modifiedTargetObject = std::format("{},{}", modifiedTargetObject, condition.m_yes);
    //             log::debug("conditional YES");
    //         } else {
    //             // if key or value not exist, add conditional "no" string
    //             modifiedTargetObject = std::format("{},{}", modifiedTargetObject, condition.m_no);
    //             log::debug("conditional NO");
    //         }
    //     }
    // } else {
    //     modifiedTargetObject = initialTargetObject;
    // }
    // auto objArray = levelLayer->createObjectsFromString(modifiedTargetObject, true, true); 
    // if (objArray->count() == 0) {
    //     log::debug("modified target object wasn't successfully created from string");
    //     showDebugText("Internal error");
    //     return;
    // }
    // auto newObj = static_cast<GameObject*>(objArray->objectAtIndex(0));
    // EditorUI::selectObject(m_fields->m_objectTarget, true);
    // EditorUI::onDeleteSelected(nullptr);
    // m_fields->m_objectTarget = newObj;

    // levelLayer->m_undoObjects->removeLastObject();
    
    // m_fields->m_objectTargetLastUse = newObj->getSaveString(nullptr);
    // EditorUI::selectObject(newObj, true);
    // EditorUI::updateButtons();
}

