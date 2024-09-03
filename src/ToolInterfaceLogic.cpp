#include "EditorUI.hpp"

void MyEditorUI::updateLineAndRect(bool clear) {
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

CCPoint MyEditorUI::calculateLineStartOnRectangle(CCPoint endPoint, std::pair<CCPoint, CCPoint> box) {
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

void MyEditorUI::controlTargetObjectCallback(float) { 
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

void MyEditorUI::resetTool() {
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
    m_fields->m_objectTargetLastUse = "";

    m_fields->m_interfaceIsVisible = false;
    m_fields->m_globalConfig.m_isFinished = false;

    EditorUI::updateButtons();
    log::debug("tool reset");
}

