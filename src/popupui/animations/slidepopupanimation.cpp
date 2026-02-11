#include "slidepopupanimation.h"

#include <qpropertyanimation.h>
#include <qdebug.h>

POPUPUI_BEGIN_NAMESPACE

SlidePopupAnimation::SlidePopupAnimation(SlideDirection dir, int fromOffset, int duration)
    : PopupAnimation(duration)
    , dir(dir)
    , fromOffset(fromOffset)
{}

QAbstractAnimation* SlidePopupAnimation::enter(QWidget* widget) {
    QRect finalGeom = widget->geometry();
    QRect startGeom = finalGeom;

    QWidget* parent = widget->parentWidget();
    if (!parent) return nullptr;

    switch (dir) {
        case SlideDirection::FromTop:
            if (fromOffset != 0) {
                startGeom.moveTop(finalGeom.top() - fromOffset);
            } else {
                startGeom.moveTop(-finalGeom.height());
            }
            break;
        case SlideDirection::FromBottom:
            if (fromOffset != 0) {
                startGeom.moveTop(finalGeom.top() + fromOffset);
            } else {
                startGeom.moveTop(parent->height());
            }
            break;
        case SlideDirection::FromLeft:
            if (fromOffset != 0) {
                startGeom.moveLeft(finalGeom.left() - fromOffset);
            } else {
                startGeom.moveLeft(-finalGeom.width());
            }
            break;
        case SlideDirection::FromRight:
            if (fromOffset != 0) {
                startGeom.moveLeft(finalGeom.left() + fromOffset);
            } else {
                startGeom.moveLeft(parent->width());
            }
            break;
    }

    widget->setGeometry(startGeom);

    auto anim = new QPropertyAnimation(widget, "geometry");
    anim->setDuration(duration);
    anim->setStartValue(startGeom);
    anim->setEndValue(finalGeom);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    return anim;
}

QAbstractAnimation* SlidePopupAnimation::exit(QWidget* widget) {
    QRect startGeom = widget->geometry();
    QRect endGeom = startGeom;

    QWidget* parent = widget->parentWidget();
    if (!parent) return nullptr;

    switch (dir) {
        case SlideDirection::FromTop:
            if (fromOffset != 0) {
                endGeom.moveTop(startGeom.top() - fromOffset);
            } else {
                endGeom.moveTop(-startGeom.height());
            }
            break;
        case SlideDirection::FromBottom:
            if (fromOffset != 0) {
                endGeom.moveTop(startGeom.top() + fromOffset);
            } else {
                endGeom.moveTop(parent->height());
            }
            break;
        case SlideDirection::FromLeft:
            if (fromOffset != 0) {
                endGeom.moveLeft(startGeom.left() - fromOffset);
            } else {
                endGeom.moveLeft(-startGeom.width());
            }
            break;
        case SlideDirection::FromRight:
            if (fromOffset != 0) {
                endGeom.moveLeft(startGeom.left() + fromOffset);
            } else {
                endGeom.moveLeft(parent->width());
            }
            break;
    }

    auto anim = new QPropertyAnimation(widget, "geometry");
    anim->setDuration(duration);
    anim->setStartValue(startGeom);
    anim->setEndValue(endGeom);
    anim->setEasingCurve(QEasingCurve::InCubic);
    return anim;
}

void SlidePopupAnimation::setParams(PopupAnimationParams* params) {
    PopupAnimation::setParams(params);
    if (auto slideParams = dynamic_cast<SlidePopupAnimationParams*>(params)) {
        dir = slideParams->dir;
        fromOffset = slideParams->fromOffset;
    }
}

POPUPUI_END_NAMESPACE
