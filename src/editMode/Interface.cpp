#include "EditLogic.hpp"
#include "../EditorUI.hpp"

void EditLogic::updateLineAndRect(bool clear) {
    if (!m_drawingLayer) {
        initDrawingLayer();
    }
    m_drawingLayer->clear();
    if (!clear) {
        m_drawingLayer->drawSegment(
            m_lineStart,
            m_lineEnd, 1.f,
            ccc4f(128, 128, 128, 155));
        m_drawingLayer->drawRect(
            m_sourceRectangle.first,
            m_sourceRectangle.second,
            ccc4f(0, 0, 0, 0), 1.f, 
            ccc4f(128, 128, 128, 155));
    }
}


void EditLogic::controlTargetObjectCallback(float) { 
    static float endObjPosX = 0.f;
    static float endObjPosY = 0.f;
    auto endObj = m_objectTarget;
    if (!endObj) return;
    if (!endObj->m_isSelected || m_editorInstance->getSelectedObjects()->count() > 1) {
        resetTool();
    } else if (endObj->getPositionX() != endObjPosX || endObj->getPositionY() != endObjPosY) {
        // if object was moved
        endObjPosX = endObj->getPositionX();
        endObjPosY = endObj->getPositionY();
        m_lineEnd = endObj->getPosition();
        m_lineStart = calculateLineStartOnRectangle(m_lineEnd, m_sourceRectangle);
        updateLineAndRect(false);
        if(m_upperMenu) {
            m_upperMenu->setPosition(endObj->getPosition() + ccp(5, 15));
        }
        if(m_lowerMenu) {
            m_lowerMenu->setPosition(endObj->getPosition() + ccp(5, -15));
        }
    }
}

CCPoint EditLogic::calculateLineStartOnRectangle(CCPoint endPoint, std::pair<CCPoint, CCPoint> box) {
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


void EditLogic::resetTool() {
    // reset menus
    if (m_upperMenu) {
        m_upperMenu->removeFromParent();
    }
    if (m_lowerMenu) {
        m_lowerMenu->removeFromParent();
    }
    // reset source objects
    if (m_objectsSource) {
        auto selectedNow = m_editorInstance->getSelectedObjects();
        for (unsigned i = 0; i < m_objectsSource->count(); i++) {
            auto obj = static_cast<GameObject*>(m_objectsSource->objectAtIndex(i));
            m_editorInstance->deselectObject(obj);
        }
        m_editorInstance->selectObjects(selectedNow, true);
    }
    // reset line
    updateLineAndRect(true);
    // reset fields
    m_upperMenu = nullptr;
    m_lowerMenu = nullptr;
    m_objectsSource = nullptr;
    m_objectsSourceCopy = nullptr;
    m_objectsSourceFiltered = nullptr;
    m_objectTarget = nullptr;
    m_objectTargetInitial = "";
    m_objectTargetLastUse = "";

    m_interfaceIsVisible = false;
    // todo: maybe add pan editor = false

    m_editorInstance->updateButtons();
    log::debug("tool reset");
}


