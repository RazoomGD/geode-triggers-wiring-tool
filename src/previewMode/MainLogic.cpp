#include "PreviewLogic.hpp"
#include "../EditorUI.hpp"

#include <chrono>
#include <ctime>  
#include <functional>

std::vector<SearchResult> getConfigForSearch(CCArray * previewObjects);

void PreviewLogic::updatePreview() {
    auto timeStart = std::chrono::system_clock::now();

    m_previewLayer->clear();

    CCArray * objects;
    auto preSelected = m_editorInstance->getSelectedObjects();
    auto allObjects = LevelEditorLayer::get()->m_objects;
    if (preSelected &&  preSelected->count()) {
        objects = preSelected;
    } else if (m_editorInstance->m_fields->m_modSettings.m_previewAllWhenNotSelected) {
        objects = allObjects;
    } else {
        m_editorInstance->showDebugText("No objects selected");
        return;
    }

    auto searchConfig = getConfigForSearch(objects);
    findObjectsOfType(allObjects, searchConfig); // this function adds the result of search to searchConfig

    for (auto& config : searchConfig) {
        if (config.m_resultObjects->count()) {
            drawPreview(config.m_queryObject, config.m_resultObjects);
        }
    }

    auto timeEnd = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsedSeconds = timeEnd - timeStart;
    log::debug("Preview for {} objects updated in {} sec", objects->count(), elapsedSeconds.count());
}

std::vector<SearchResult> getConfigForSearch(CCArray * queryObjects) {
    std::vector<SearchResult> result;
    for (unsigned i = 0; i < queryObjects->count(); i++) {
        auto obj = static_cast<GameObject*>(queryObjects->objectAtIndex(i));
        auto objConfig = PREVIEW_CONFIG.find(obj->m_objectID);
        if (objConfig != PREVIEW_CONFIG.end()) {
            std::stringstream ss(obj->getSaveString(nullptr));
            std::map<std::string, std::string> kvObject;
            std::string key, val;
            while (std::getline(ss, key, ',') && std::getline(ss, val, ',')) {
                kvObject.insert({key, val});
            }
            for (auto& config : objConfig->second) {
                bool goodObj = true;
                for (auto& condition : config.m_conditions) {
                    auto value = kvObject.find(condition.m_conditionKey);
                    if (value != kvObject.end()) {
                        if (condition.m_conditionValue != "" && condition.m_conditionValue != (*value).second) {
                            goodObj = false;
                            break;
                        }
                    } else if (condition.m_conditionValue != "0") {
                        goodObj = false;
                        break;
                    }
                }
                if (!goodObj) continue;

                auto searchVal = kvObject.find(config.m_key);
                if (searchVal == kvObject.end()) continue;
                
                if (config.m_type == fieldType::groupMapField) {
                    std::stringstream ss((*searchVal).second);
                    std::string key, val;
                    while (std::getline(ss, key, '.') && std::getline(ss, val, '.')) {
                        short num = std::stoi(key);
                        result.push_back(SearchResult(searchType::groupSearch, num, obj));
                    }
                } else {
                    searchType type;
                    switch (config.m_type) {
                        case fieldType::groupIdField: 
                            type = searchType::groupSearch;
                            break;
                        case fieldType::itemIdField: 
                            type = searchType::itemSearch;
                            break;
                        case fieldType::collisionBlockIdField: 
                            type = searchType::collisionBlockSearch;
                            break;
                        default: 
                            type = searchType::groupSearch; 
                            break;
                    }
                    short num = std::stoi((*searchVal).second);
                    result.push_back(SearchResult(type, num, obj));
                }
            }
        } 
    }
    return result;
}

void PreviewLogic::findObjectsOfType(CCArray * const objects, std::vector<SearchResult> &categories) {

    // group id + type -- arrays to add objects to    
    std::map<std::pair<short, searchType>, std::vector<CCArray *>> searchedObjects;

    for (auto& el : categories) {
        std::pair<short, searchType> mapKey({el.m_number, el.m_searchType});
        auto searchGr = searchedObjects.find(mapKey);
        if (searchGr == searchedObjects.end()) {
            searchedObjects.insert({mapKey, {el.m_resultObjects}});
        } else {
            searchGr->second.push_back(el.m_resultObjects);
        }
    }

    for (unsigned i = 0; i < objects->count(); i++) {
        auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
        // groups
        if (obj->m_groups) {
            for (unsigned i = 0; i < obj->m_groupCount; i++) {
                short group = obj->m_groups->at(i);
                auto searchGr = searchedObjects.find(std::pair{group, searchType::groupSearch});
                if (searchGr != searchedObjects.end()) {
                    for (auto& objArray : searchGr->second) {
                        objArray->addObject(obj);
                    }
                }
            }
        }
        if (obj->m_objectID == itemObjID || obj->m_objectID == collisionBlockID) {
            auto itemId = static_cast<EffectGameObject*>(obj)->m_itemID; // this is collision id as well
            auto type = (obj->m_objectID == itemObjID) ? searchType::itemSearch : searchType::collisionBlockSearch;
            auto searchItem = searchedObjects.find(std::pair{itemId, type});
            if (searchItem != searchedObjects.end()) {
                for (auto& objArray : searchItem->second) {
                    objArray->addObject(obj);
                }
            }
        }
    }
}


void PreviewLogic::drawPreview(GameObject * const center, CCArray * const sourceObjects) {
    // get source objects box
    int leftX = INT_MAX; 
    int topY = INT_MIN;
    int rightX = INT_MIN;
    int bottomY = INT_MAX;
    unsigned srcObjUniqSum = 0; // for the same random color for same source objects
    for (unsigned i = 0; i < sourceObjects->count(); i++) {
        auto obj = static_cast<GameObject*>(sourceObjects->objectAtIndex(i));
        auto bound = obj->boundingBox();
        if (bound.getMaxX() > rightX) rightX = bound.getMaxX();
        if (bound.getMinX() < leftX) leftX = bound.getMinX();
        if (bound.getMaxY() > topY) topY = bound.getMaxY();
        if (bound.getMinY() < bottomY) bottomY = bound.getMinY();
        // for random color
        if (m_editorInstance->m_fields->m_modSettings.m_previewModeColorfulLines) {
            srcObjUniqSum += obj->m_uniqueID;
            srcObjUniqSum = (srcObjUniqSum >> 5) | (srcObjUniqSum << (sizeof(unsigned)*8 - 5)); // rotate
        }
    }
    auto topLeft = ccp(leftX, topY);
    auto bottomRight = ccp(rightX, bottomY);
    auto lineStart = center->getPosition();
    std::optional<int> randomColorSeed = srcObjUniqSum ? srcObjUniqSum : std::optional<int>{};
    drawLineAndRectangle(lineStart, topLeft - ccp(3, -3), bottomRight + ccp(3, -3), randomColorSeed);
}

void PreviewLogic::drawLineAndRectangle(const CCPoint &startLine, const CCPoint &topLeftRect, 
    const CCPoint &bottomRightRect, std::optional<int> randomColorSeed) {
    // get line end on rect
    CCPoint lineEnd;
    auto lineStartX = startLine.x;
    auto lineStartY = startLine.y;
    short isPointInside = 0;
    if (lineStartX > bottomRightRect.x) lineStartX = bottomRightRect.x;
    else if (lineStartX < topLeftRect.x) lineStartX = topLeftRect.x;
    else isPointInside++;
    if (lineStartY > topLeftRect.y) lineStartY = topLeftRect.y;
    else if (lineStartY < bottomRightRect.y) lineStartY = bottomRightRect.y;
    else isPointInside++;
    if (isPointInside == 2) {
        // nearest side
        auto toLeft = startLine.x - topLeftRect.x;
        auto toRight = bottomRightRect.x - startLine.x;
        auto toTop = topLeftRect.y - startLine.y;
        auto toBottom = startLine.y - bottomRightRect.y;
        auto minDist = toLeft;
        lineEnd = ccp(topLeftRect.x, startLine.y);
        if (toRight < minDist) {
            minDist = toRight;
            lineEnd = ccp(bottomRightRect.x, startLine.y);
        }
        if (toTop < minDist) {
            minDist = toTop;
            lineEnd = ccp(startLine.x, topLeftRect.y);
        }
        if (toBottom < minDist) {
            minDist = toBottom;
            lineEnd = ccp(startLine.x, bottomRightRect.y);
        }
    } else {
        lineEnd = ccp(lineStartX, lineStartY);
    }

    union {
        uint8_t values[3] = {128, 128, 128};
        size_t random;
    } rgb;
    if (randomColorSeed) {
        rgb.random = std::hash<int>{}(*randomColorSeed);
    }
    auto color = ccc4f(rgb.values[0], rgb.values[1], rgb.values[2], 155);
    m_previewLayer->drawRect(topLeftRect, bottomRightRect, 
        ccc4f(0, 0, 0, 0), 1.f, color);

    m_previewLayer->drawSegment(startLine, lineEnd, 1.f, color);
    // auto startPoint = ccp(lineStartX, lineStartY);
    // auto tmp = ccp(50, 50);
    // CCPoint line[] = {tmp, startPoint, lineEnd};
    // m_previewLayer->drawLines(line, 2, 1.f, color);
    // m_previewLayer->drawPolygon(line, 3, ccc4f(0,0,0,0), 1.f, color);
    // log::debug("draw poligon");
}


