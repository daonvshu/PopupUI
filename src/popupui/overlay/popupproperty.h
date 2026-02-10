#pragma once

#include <popupui/comm/global.h>
#include <qwidget.h>

POPUPUI_BEGIN_NAMESPACE

struct POPUPUI_EXPORT PopupProperty {
    bool closable = false;
    QColor color = Qt::transparent;
    bool interceptMouse = true;
    bool deleteOnClose = true;
    QWidget* draggableWidget = nullptr;
    bool boundedDrag = true;
    bool baseMaskVisible = true;

    Qt::Alignment posAlign = Qt::AlignHCenter;
    Qt::Edge edgeAlign = Qt::TopEdge;
    QWidget* alignToTarget = nullptr;
    Qt::Alignment alignToPos = Qt::AlignLeft | Qt::AlignBottom;
    std::function<QPoint()> alignToPosProvider;
    QPoint alignOffset;

    double widthAspectRatio = -1;
    double heightAspectRatio = -1;

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

    PopupProperty& setDraggableArea(QWidget* draggableAreaWidget, bool bounded = true) {
        this->draggableWidget = draggableAreaWidget;
        this->boundedDrag = bounded;
        return *this;
    }

    PopupProperty& showBaseMaskLayer(bool show = true) {
        this->baseMaskVisible = show;
        return *this;
    }

    PopupProperty& alignToWidget(QWidget* target, Qt::Alignment targetPos, Qt::Edge edgeAlign = Qt::TopEdge, Qt::Alignment posAlign = Qt::AlignHCenter, const QPoint& offset = QPoint(0, 0)) {
        this->alignToTarget = target;
        this->alignToPos = targetPos;
        this->edgeAlign = edgeAlign;
        this->posAlign = posAlign;
        this->alignOffset = offset;
        return *this;
    }

    PopupProperty& alignToWidget(QWidget* target, const std::function<QPoint()>& targetPosProvider, Qt::Edge edgeAlign = Qt::TopEdge, Qt::Alignment posAlign = Qt::AlignHCenter, const QPoint& offset = QPoint(0, 0)) {
        this->alignToTarget = target;
        this->alignToPosProvider = targetPosProvider;
        this->edgeAlign = edgeAlign;
        this->posAlign = posAlign;
        this->alignOffset = offset;
        return *this;
    }

    PopupProperty& keepAspectRatio(double widthRatio, double heightRatio) {
        this->widthAspectRatio = widthRatio;
        this->heightAspectRatio = heightRatio;
        return *this;
    }
};

POPUPUI_END_NAMESPACE