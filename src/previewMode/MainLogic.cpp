#include "PreviewLogic.hpp"

#include <chrono>
#include <ctime>  
#include <functional>

std::vector<SearchResult> getConfigForSearch(CCArray * previewObjects);

void PreviewLogic::updatePreview() {
    auto timeStart = std::chrono::system_clock::now();
    
    m_previewLayer->clear();
    auto objects = LevelEditorLayer::get()->m_objects;
    log::debug("objects total: {}", objects->count());

    auto searchConfig = getConfigForSearch(objects);
    findObjectsOfType(objects, searchConfig);
    for (auto& config : searchConfig) {
        if (config.m_resultObjects->count()) {
            drawPreview(config.m_queryObject, config.m_resultObjects);
        }
    }

    auto timeEnd = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsedSeconds = timeEnd - timeStart;
    log::debug("Preview updated in {} sec", elapsedSeconds.count());
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
                // todo: map type type checks
                auto searchVal = kvObject.find(config.m_key);
                if (searchVal != kvObject.end()) {
                    short num = std::stoi((*searchVal).second);
                    // todo: other types
                    result.push_back(SearchResult(searchType::groupSearch, num, obj));
                }
            }
        } 
    }
    return result;
}

void PreviewLogic::findObjectsOfType(CCArray * const objects, std::vector<SearchResult> &categories) {
    // group id + arrays to add objects to
    // todo сделать ключом pair<objid, type>
    std::map<short, std::vector<CCArray *>> searchedGroups;
    for (auto& el : categories) {
        switch (el.m_searchType) {
            case searchType::groupSearch: {
                auto searchGr = searchedGroups.find(el.m_number);
                if (searchGr == searchedGroups.end()) {
                    searchedGroups.insert({el.m_number, {el.m_resultObjects}});
                } else {
                    searchGr->second.push_back(el.m_resultObjects);
                }
                break;
            }
            default: break;
        }

    }

    for (unsigned i = 0; i < objects->count(); i++) {
        auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
        // groups
        if (obj->m_groups) {
            for (unsigned i = 0; i < obj->m_groupCount; i++) {
                short group = obj->m_groups->at(i);
                auto searchGr = searchedGroups.find(group);
                if (searchGr != searchedGroups.end()) {
                    for (auto& objArray : searchGr->second) {
                        objArray->addObject(obj);
                    }
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
    auto lineStart = center->getPosition();
    // todo: setting for this
    std::optional<int> randomColorSeed = true ? center->m_uniqueID : std::optional<int>{};
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

    m_previewLayer->drawSegment(startLine,lineEnd, 1.f, color);
}


