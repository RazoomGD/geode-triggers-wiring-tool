#include "EditorUI.hpp"

std::set<srcObjType> MyEditorUI::getTypesById(short objId) {
    std::set<srcObjType> result;
    if (triggerIDs.contains(objId)) {
        result.insert(srcObjType::trig);
    } 
    if (animatedIDs.contains(objId)) {
        result.insert(srcObjType::anim);
    }
    if (objId == keyFrameOjb) {
        result.insert(srcObjType::keyFrame);
    } 
    result.insert(srcObjType::any);
    return result;
}

CCArray * MyEditorUI::filterObjectsByType(std::set<srcObjType> filteringTypes, CCArray * objects, bool color) {
    auto result = CCArray::create();
    for (unsigned i = 0; i < objects->count(); i++) {
        auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
        auto objTypes = getTypesById(obj->m_objectID);
        // to be accepted object must have all filtering types
        bool accept = true;
        for (auto &type : filteringTypes) {
            if (!objTypes.contains(type)) {
                accept = false;
                break;
            }
        }
        if (accept) {
            result->addObject(obj);
            if (color) obj->selectObject(ccc3(255, 0, 255));
        } else {
            if (color) EditorUI::deselectObject(obj);
        }
    }
    return result;
}

// std::set<srcObjType> MyEditorUI::getObjectsAllTypes(CCArray * objects) {
//     std::set<srcObjType> presentTypes;
//     for (unsigned i = 0; i < objects->count(); i++) {
//         auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
//         auto types = getTypesById(obj->m_objectID);
//         presentTypes.insert(types.begin(), types.end());
//     }
//     return presentTypes;
// }

std::vector<short> MyEditorUI::getObjectsCommonGroups(CCArray * objects) {
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
};

bool MyEditorUI::isNewGroupPossible(CCArray * objects) {
    for (unsigned i = 0; i < objects->count(); i++) {
        auto groups = static_cast<GameObject*>(objects->objectAtIndex(i))->m_groups;
        if (!groups) continue;
        if (groups->at(9) != 0) {  
            return false;
        }
    }
    return true;
};

void MyEditorUI::addToGroup(int group, CCArray * objects) {
    for (unsigned i = 0; i < objects->count(); i++) {
        auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
        obj->addToGroup(group);
    }
};

void MyEditorUI::addToGroupSM(int group, CCArray * objects) {
    for (unsigned i = 0; i < objects->count(); i++) {
        auto obj = static_cast<EffectGameObject*>(objects->objectAtIndex(i));
        obj->addToGroup(group);
        obj->m_isSpawnTriggered = true;
        obj->m_isMultiTriggered = true;
    }
};

void MyEditorUI::addToGroupAnim(int group, CCArray * objects) {
    for (unsigned i = 0; i < objects->count(); i++) {
        auto obj = static_cast<EnhancedGameObject*>(objects->objectAtIndex(i));
        obj->addToGroup(group);
        // it breaks with these ids
        if (!animatedSpecialIDs.contains(obj->m_objectID)) obj->m_animateOnTrigger = true;
    }
};

// copy 1)groups todo: 2)spawn, multi trigger, itemID, animate on trigger
void MyEditorUI::myCopyObjectProps(GameObject * from, TWTObjCopy * to) {
    if (from->m_groups) {
        std::memcpy(to->m_groups, from->m_groups, sizeof(short) * 10);
    } else {
        to->m_groups->fill(0);
    }
    to->m_groupCount = from->m_groupCount;
    auto objTypes = getTypesById(from->m_objectID);
    if (objTypes.contains(srcObjType::trig)) {
        to->m_isSpawnTrigger = static_cast<EffectGameObject*>(from)->m_isSpawnTriggered;
        to->m_isMultiTrigger = static_cast<EffectGameObject*>(from)->m_isMultiTriggered;
    }
    if (objTypes.contains(srcObjType::anim)) {
        to->m_isAnimOnTrigger = static_cast<EnhancedGameObject*>(from)->m_animateOnTrigger;
    }  
};

void MyEditorUI::myPasteObjectProps(TWTObjCopy * from, GameObject * to) {
    if (!to->m_groups) to->addToGroup(1);
    std::memcpy(to->m_groups, from->m_groups, sizeof(short) * 10);
    to->m_groupCount = from->m_groupCount;
    if (from->m_isSpawnTrigger) {
        static_cast<EffectGameObject*>(to)->m_isSpawnTriggered = *(from->m_isSpawnTrigger);
    }
    if (from->m_isMultiTrigger) {
        static_cast<EffectGameObject*>(to)->m_isMultiTriggered = *(from->m_isMultiTrigger);
    }
    if (from->m_isAnimOnTrigger) {
        static_cast<EnhancedGameObject*>(to)->m_animateOnTrigger = *(from->m_isAnimOnTrigger);
    }
}

std::vector<int> MyEditorUI::getObjectsAllColors(CCArray * objects) {
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
};

std::optional<int> MyEditorUI::getCommonBaseColor(CCArray * objects) {
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
};

std::optional<int> MyEditorUI::getCommonDetailColor(CCArray * objects) {
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
};

std::map<std::string, std::string> MyEditorUI::objectToKeyVal(std::string objSaveString) {
    std::stringstream ss(objSaveString);
    std::map<std::string, std::string> keyVals;
    std::string key, val;
    while (std::getline(ss, key, ',') && std::getline(ss, val, ',')) {
        keyVals.insert({key, val});
    }
    return keyVals;
};

// needed to check if something was changed in targetObj via editObject menu or somehow else
// while tool interface was open. This function computes this difference and applies
// it to the "obj" witch is an initial state of targetObj
std::string MyEditorUI::applyDifference(std::string before, std::string after, std::string obj) {
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
};

