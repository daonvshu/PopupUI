#pragma once

#include <popupui/comm/global.h>
#include <qwidget.h>

POPUPUI_BEGIN_NAMESPACE

struct POPUPUI_EXPORT PopupProperty {
    bool closable = false;
    QColor color = Qt::transparent;
    bool interceptMouse = true;
    bool deleteOnClose = true;
    bool keepCenter = true;
    QWidget* draggableWidget = nullptr;
    bool boundedDrag = true;
    bool baseMaskVisible = true;

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

    PopupProperty& setDraggableArea(QWidget* draggableAreaWidget, bool bounded = true) {
        this->draggableWidget = draggableAreaWidget;
        this->boundedDrag = bounded;
        this->keepCenter = false;
        return *this;
    }

    PopupProperty& showBaseMaskLayer(bool show = true) {
        this->baseMaskVisible = show;
        return *this;
    }
};

POPUPUI_END_NAMESPACE