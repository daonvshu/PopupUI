#pragma once

#include <popupui/comm/global.h>

POPUPUI_BEGIN_NAMESPACE

struct PopupProperty {
    bool closable = false;
    QColor color = Qt::transparent;
    bool interceptMouse = true;
    bool deleteOnClose = true;
    bool keepCenter = true;
    QRectF draggableArea;
    bool boundedDrag = true;

    PopupProperty& closeOnClickOutside(bool enabled = true) {
        this->closable = enabled;
        return *this;
    }

    PopupProperty& setMaskColor(const QColor& maskColor) {
        this->color = maskColor;
        return *this;
    }

    PopupProperty& transparentMouseEvent(bool enabled = true) {
        this->interceptMouse = !enabled;
        return *this;
    }

    PopupProperty& setDeleteOnClose(bool enabled = true) {
        this->deleteOnClose = enabled;
        return *this;
    }

    PopupProperty& keepCenterOnResized(bool enabled = true) {
        this->keepCenter = enabled;
        return *this;
    }

    PopupProperty& setDraggableArea(const QRectF& area, bool bounded = true) {
        this->draggableArea = area;
        this->boundedDrag = bounded;
        this->keepCenter = false;
        return *this;
    }
};

POPUPUI_END_NAMESPACE