#include "EditorUI.hpp"

bool MyEditorUI::handleTouchStart(const CCTouch * const touch) {
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

void MyEditorUI::handleTouchMiddle(const CCTouch * const touch) {
    m_fields->m_lineEnd = screenToEditorLayerPosition(touch->getLocationInView());
    m_fields->m_lineStart = calculateLineStartOnRectangle(m_fields->m_lineEnd, m_fields->m_sourceRectangle);
    updateLineAndRect(false);
}

void MyEditorUI::handleTouchEnd(bool select, bool showDebug) {
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