#include "Utils.hpp"

std::set<srcObjType> getTypesById(short objId) {
    std::set<srcObjType> result;
    if (triggerIDs.contains(objId)) result.insert(srcObjType::trig);
    if (animatedIDs.contains(objId)) result.insert(srcObjType::anim);
    if (objId == keyFrameOjbID) result.insert(srcObjType::keyFrame);
    if (objId == itemObjID) result.insert(srcObjType::item);
    if (objId == particleObjID) result.insert(srcObjType::particle);
    if (objId == sfxTriggerObjID) result.insert(srcObjType::particle);
    if (objId == gradientTriggerId) result.insert(srcObjType::gradientTrig);
    if (objId == collisionBlockID) result.insert(srcObjType::collision);
    if (areaEffectsIDs.contains(objId)) result.insert(srcObjType::areaEffect);
    if (collectableIDs.contains(objId)) result.insert(srcObjType::collectable);
    result.insert(srcObjType::any); // any object is any
    return result;
}

CCArray * filterObjectsByType(srcObjType filteringType, CCArray * objects, bool color, EditorUI * editorInstance) {
    auto result = CCArray::create();
    for (unsigned i = 0; i < objects->count(); i++) {
        auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
        auto objTypes = getTypesById(obj->m_objectID);
        if (objTypes.contains(filteringType)) {
            result->addObject(obj);
            if (color) obj->selectObject(ccc3(255, 0, 255));
        } else {
            if (color) editorInstance->deselectObject(obj);
        }
    }
    return result;
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
};

bool isNewGroupPossible(CCArray * objects) {
    for (unsigned i = 0; i < objects->count(); i++) {
        auto groups = static_cast<GameObject*>(objects->objectAtIndex(i))->m_groups;
        if (!groups) continue;
        if (groups->at(9) != 0) {
            return false;
        }
    }
    return true;
};

bool isGroupPossible(CCArray * objects, int group) {
    for (unsigned i = 0; i < objects->count(); i++) {
        auto groups = static_cast<GameObject*>(objects->objectAtIndex(i))->m_groups;
        if (!groups) continue;
        if (groups->at(9) != 0) {
            if (std::find(groups->begin(), groups->end(), group) == groups->end()) {
                return false;
            }
        }
    }
    return true;
};

void addToGroup(int group, CCArray * objects) {
    for (unsigned i = 0; i < objects->count(); i++) {
        auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
        obj->addToGroup(group);
    }
};

void addToGroupSM(int group, CCArray * objects) {
    for (unsigned i = 0; i < objects->count(); i++) {
        auto obj = static_cast<EffectGameObject*>(objects->objectAtIndex(i));
        obj->addToGroup(group);
        obj->m_isSpawnTriggered = true;
        obj->m_isMultiTriggered = true;
    }
};

void addToGroupAnim(int group, CCArray * objects) {
    for (unsigned i = 0; i < objects->count(); i++) {
        auto obj = static_cast<EnhancedGameObject*>(objects->objectAtIndex(i));
        obj->addToGroup(group);
        // it breaks with these ids
        if (!animatedSpecialIDs.contains(obj->m_objectID)) obj->m_animateOnTrigger = true;
    }
};

void setItemId(int id, CCArray * objects) {
    for (unsigned i = 0; i < objects->count(); i++) {
        auto obj = static_cast<EffectGameObject*>(objects->objectAtIndex(i));
        obj->m_itemID = id;
        static_cast<CCLabelBMFont*>(obj->getChildren()->objectAtIndex(0))
            ->setString((std::format("C:{}", id).c_str()));
    }
}

void setCollisionId(int id, CCArray * objects) {
    for (unsigned i = 0; i < objects->count(); i++) {
        auto obj = static_cast<EffectGameObject*>(objects->objectAtIndex(i));
        obj->m_itemID = id;
        bool isDyn = obj->m_isDynamicBlock;
        static_cast<CCLabelBMFont*>(obj->getChildren()->objectAtIndex(0))
            ->setString((std::format("{}{}", id, (isDyn ? "." : "")).c_str()));
    }
}

void setGradientId(int id, CCArray * objects) {
    for (unsigned i = 0; i < objects->count(); i++) {
        setGradientTriggerId(objects->objectAtIndex(i), id);
    }
}

// copy 1)groups todo: 2)spawn, multi trigger, itemID, animate on trigger
void myCopyObjectProps(GameObject * from, TWTObjCopy * to) {
    if (from->m_groups) {
        std::memcpy(to->m_groups, from->m_groups, sizeof(short) * 10);
    } else {
        to->m_groups->fill(0);
    }
    to->m_groupCount = from->m_groupCount;
    auto objTypes = getTypesById(from->m_objectID);
    // optional values
    if (objTypes.contains(srcObjType::trig)) {
        to->m_isSpawnTrigger = static_cast<EffectGameObject*>(from)->m_isSpawnTriggered;
        to->m_isMultiTrigger = static_cast<EffectGameObject*>(from)->m_isMultiTriggered;
    }
    if (objTypes.contains(srcObjType::anim)) {
        to->m_isAnimOnTrigger = static_cast<EnhancedGameObject*>(from)->m_animateOnTrigger;
    }
    if (objTypes.contains(srcObjType::item)) {
        to->m_itemID = static_cast<EffectGameObject*>(from)->m_itemID;
    }
    if (objTypes.contains(srcObjType::collision)) {
        to->m_collisionID = static_cast<EffectGameObject*>(from)->m_itemID;
        to->m_isCollisionDyn = static_cast<EffectGameObject*>(from)->m_isDynamicBlock;
    }
    if (objTypes.contains(srcObjType::gradientTrig)) {
        to->m_gradientID = getGradientTriggerId(from);
    }
};

void myPasteObjectProps(TWTObjCopy * from, GameObject * to) {
    if (!to->m_groups) to->addToGroup(1);
    std::memcpy(to->m_groups, from->m_groups, sizeof(short) * 10);
    to->m_groupCount = from->m_groupCount;
    // optional values
    if (from->m_isSpawnTrigger) {
        static_cast<EffectGameObject*>(to)->m_isSpawnTriggered = *(from->m_isSpawnTrigger);
    }
    if (from->m_isMultiTrigger) {
        static_cast<EffectGameObject*>(to)->m_isMultiTriggered = *(from->m_isMultiTrigger);
    }
    if (from->m_isAnimOnTrigger) {
        static_cast<EnhancedGameObject*>(to)->m_animateOnTrigger = *(from->m_isAnimOnTrigger);
    }
    if (from->m_itemID) {
        static_cast<EffectGameObject*>(to)->m_itemID = *(from->m_itemID);
        static_cast<CCLabelBMFont*>(to->getChildren()->objectAtIndex(0))
            ->setString((std::format("C:{}", *(from->m_itemID)).c_str()));
    }
    if (from->m_collisionID) {
        bool isDyn = from->m_isCollisionDyn ? *(from->m_isCollisionDyn) : false;
        static_cast<EffectGameObject*>(to)->m_itemID = *(from->m_collisionID);
        static_cast<EffectGameObject*>(to)->m_isDynamicBlock = isDyn;
        static_cast<CCLabelBMFont*>(to->getChildren()->objectAtIndex(0))
            ->setString((std::format("{}{}", *(from->m_collisionID), (isDyn ? "." : "")).c_str()));
    }
    if (from->m_gradientID) {
        setGradientTriggerId(to, *(from->m_gradientID));
    }
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
};

std::vector<int> getItemsAllIds(CCArray * objects) {
    std::set<int> itemIds;
    for (unsigned i = 0; i < objects->count(); i++) {
        auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
        if (obj->m_objectID != itemObjID) continue;
        auto item = static_cast<EffectGameObject*>(objects->objectAtIndex(i));
        itemIds.insert(item->m_itemID);
    }
    return std::vector<int>(itemIds.begin(), itemIds.end());
};

std::vector<int> getCollisionsAllIds(CCArray * objects) {
    std::set<int> collisionIds;
    for (unsigned i = 0; i < objects->count(); i++) {
        auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
        if (obj->m_objectID != collisionBlockID) continue;
        auto colBlock = static_cast<EffectGameObject*>(objects->objectAtIndex(i));
        collisionIds.insert(colBlock->m_itemID);
    }
    return std::vector<int>(collisionIds.begin(), collisionIds.end());
};

std::vector<int> getGradientsAllIds(CCArray * objects) {
    std::set<int> gradIds;
    for (unsigned i = 0; i < objects->count(); i++) {
        auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
        if (obj->m_objectID != gradientTriggerId) continue;
        auto id = getGradientTriggerId(obj);
        gradIds.insert(id);
    }
    return std::vector<int>(gradIds.begin(), gradIds.end());
};

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
};

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
};

int getNextFreeItemFixed() {
    // Robtop's function is broken. This one is slow but at least works
    auto levelLayer = LevelEditorLayer::get();
    auto objects = levelLayer->m_objects;
    std::set<short> itemIds;
    for (unsigned i = 0; i < objects->count(); i++) {
        auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
        if (itemIdContainingObjects.contains(obj->m_objectID)) {
            auto effectObj = static_cast<EffectGameObject*>(obj);
            if (effectObj->m_itemID > 0 && effectObj->m_collectibleIsPickupItem) {
                itemIds.insert(effectObj->m_itemID);
            }
        } else {
            auto kvObject = objectToKeyVal(obj->getSaveString(nullptr));
            switch (obj->m_objectID) {
                case 1817:   // pickup trigger
                case 1611:   // count trigger 
                case 1811:   // instant count trigger 
                case 3641:   // persist trigger
                case 3617:   // time control trigger
                case 3615:   // time event trigger
                case 3614:   // time trigger
                case 1615: { // item itself
                    if (kvObject.contains("80")) itemIds.insert(std::stoi(kvObject["80"]));
                    break;
                };
                case 3619: { // edit item trigger
                    if (kvObject.contains("80") && kvObject.contains("476") && kvObject["476"] == "1") 
                        itemIds.insert(std::stoi(kvObject["80"]));
                    if (kvObject.contains("95") && kvObject.contains("477") && kvObject["477"] == "1") 
                        itemIds.insert(std::stoi(kvObject["95"]));
                    if (kvObject.contains("51") && kvObject.contains("478") && kvObject["478"] == "1") 
                        itemIds.insert(std::stoi(kvObject["51"]));
                    break;
                };
                case 3620: { // compare item trigger
                    if (kvObject.contains("80")) itemIds.insert(std::stoi(kvObject["80"]));
                    if (kvObject.contains("95") && kvObject.contains("477") && kvObject["477"] == "1") 
                        itemIds.insert(std::stoi(kvObject["95"]));
                    break;
                };
                default: break;
            }
        }
    }
    int current = 1;
    while (itemIds.contains(current)) {
        current++;
    }
    return current;
}

// original function was inlined (0x89e40)
int getNextFreeBlock() {
    auto levelLayer = LevelEditorLayer::get();
    auto objects = levelLayer->m_objects;
    std::set<short> blockIds;
    for (unsigned i = 0; i < objects->count(); i++) {
        auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
        if (obj->m_objectID == collisionBlockID) {
            auto blockId = static_cast<EffectGameObject*>(obj)->m_itemID;
            blockIds.insert(blockId);
        }
    }
    int current = 1;
    while (blockIds.contains(current)) {
        current++;
    }
    return current;
}

inline int getGradientTriggerId(CCObject * trig) { // todo: test it for android
    // completely safe
    auto ptr = reinterpret_cast<char*>(trig);
    auto id = *reinterpret_cast<int32_t*>(ptr + 0x748);
    return id;
}

inline void setGradientTriggerId(CCObject * trig, int32_t id) {
    // completely safe 2
    auto ptr = reinterpret_cast<char*>(trig);
    *reinterpret_cast<int32_t*>(ptr + 0x748) = id;
}

std::map<std::string, std::string> objectToKeyVal(std::string objSaveString) {
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
};





// auto myWrapFunction(uintptr_t address, tulip::hook::WrapperMetadata const& metadata) {
// 	auto wrapped = geode::hook::createWrapper(reinterpret_cast<void*>(address), metadata);
// 	if (wrapped.isErr()) {{
// 		throw std::runtime_error(wrapped.unwrapErr());
// 	}}
// 	return wrapped.unwrap();
// }


// void myHook(long long smth) {
//     log::info("Hook reached!");
// 	// Call the original manually
// 	static auto original = myWrapFunction(
//         geode::base::get() + 0x3d1eb0,
// 		tulip::hook::WrapperMetadata{
// 			.m_convention = geode::hook::createConvention(tulip::hook::TulipConvention::Fastcall),
// 			.m_abstract = tulip::hook::AbstractFunction::from(static_cast<void(*)(long long)>(nullptr)),
// 		}
//     );
//     reinterpret_cast<void(*)(long long)>(original)(smth);
//     log::info("After original!");
// }

// $execute {
//     Mod::get()->hook(
//         reinterpret_cast<void*>(geode::base::get() + 0x3d1540), // address
//         &myHook, // detour
//         "MenuLayer::onNewgrounds", // display name, shows up on the console
//         tulip::hook::TulipConvention::Thiscall // calling convention
//     );
// }

