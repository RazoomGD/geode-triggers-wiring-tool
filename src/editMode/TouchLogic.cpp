#include "EditLogic.hpp"
#include "../EditorUI.hpp"

bool EditLogic::handleTouchStart(const CCTouch * const touch, bool forceActivateOnce) {
    if (forceActivateOnce) m_toolActivatedForOnce = true;

    if (!toolIsActivated()) return false;

    if (m_interfaceIsVisible) resetTool();

    m_lineStart = m_editorInstance->screenToEditorLayerPosition(touch->getLocationInView());
    m_lineEnd = m_lineStart;

    auto preSelected = m_editorInstance->getSelectedObjects();

    // if we have previously selected objects
    // we check if we've started line from them
    bool useSelectedFlag = false;
    if (preSelected->count()) {
        for (unsigned i = 0; i < preSelected->count(); i++) {
            auto obj = static_cast<GameObject*>(preSelected->objectAtIndex(i));
            auto objBox = obj->boundingBox();
            auto click = m_lineEnd;
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
        auto startObj = LevelEditorLayer::get()->objectAtPosition(m_lineEnd);
        if (startObj) {
            m_editorInstance->selectObject(startObj, false);
            sourceObjects = m_editorInstance->getSelectedObjects();
        } else if (preSelected->count()) {
            sourceObjects = preSelected;
        }
    }

    if (sourceObjects && sourceObjects->count()) {
        m_editorInstance->deselectAll();
        for (unsigned i = 0; i < sourceObjects->count(); i++) {
            auto obj = static_cast<GameObject*>(sourceObjects->objectAtIndex(i));
            obj->selectObject(ccc3(255, 0, 255));
        }
        m_objectsSource = sourceObjects;
        m_editorInstance->showDebugText(std::format("Selected objects: {}", sourceObjects->count()));
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
        m_sourceRectangle.first = topLeft - ccp(3, -3);
        m_sourceRectangle.second = bottomRight + ccp(3, -3);

        m_lineStart = calculateLineStartOnRectangle(m_lineEnd, m_sourceRectangle);
        updateLineAndRect(false);
        m_interfaceIsVisible = true;
        return true;
    }
    m_editorInstance->showDebugText("No objects selected");
    m_toolActivatedForOnce = false;
    return false;
}

bool EditLogic::handleTouchMiddle(const CCTouch * const touch) {

    if (toolIsActivated() && m_interfaceIsVisible) {
        m_lineEnd = m_editorInstance->screenToEditorLayerPosition(touch->getLocationInView());
        m_lineStart = calculateLineStartOnRectangle(m_lineEnd, m_sourceRectangle);
        updateLineAndRect(false);
        return true;
    } else {
        if (m_interfaceIsVisible && !m_objectTarget) {
            resetTool(); 
        }
        return false;
    } 
}

bool EditLogic::handleTouchEnd(bool select, bool showDebug) {

    if (toolIsActivated() && m_interfaceIsVisible) {
        if (m_objectsSource) {
            auto endObj = LevelEditorLayer::get()->objectAtPosition(m_lineEnd);
            if (select && endObj) {
                m_lineEnd = endObj->getPosition();
                m_lineStart = calculateLineStartOnRectangle(m_lineEnd, m_sourceRectangle);
                updateLineAndRect(false);
                m_objectTarget = endObj;
                handleTargetObject();
            } else {
                if (showDebug) m_editorInstance->showDebugText("No objects selected");
                resetTool();
            }
        } else {
            resetTool();
        }
        
        if (m_editorInstance->m_fields->m_modSettings.m_autoDisable && m_buttonIsActivated) 
            onMainButton(m_editorInstance->m_fields->m_editButton, false);
        m_toolActivatedForOnce = false;
        return true;
    } else {
        if (m_interfaceIsVisible && !m_objectTarget) {
            resetTool();
        }
        m_toolActivatedForOnce = false;
        return false;
    }    
}


