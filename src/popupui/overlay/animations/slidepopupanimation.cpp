#include "slidepopupanimation.h"

#include <qpropertyanimation.h>

POPUPUI_BEGIN_NAMESPACE

SlidePopupAnimation::SlidePopupAnimation(SlideDirection dir, int duration)
    : PopupAnimation(duration)
    , dir(dir)
{}

QAbstractAnimation* SlidePopupAnimation::enter(QWidget* widget) {
    QRect finalGeom = widget->geometry();
    QRect startGeom = finalGeom;

    QWidget* parent = widget->parentWidget();
    if (!parent) return nullptr;

    switch (dir) {
        case SlideDirection::FromTop:
            startGeom.moveTop(-finalGeom.height());
            break;
        case SlideDirection::FromBottom:
            startGeom.moveTop(parent->height());
            break;
        case SlideDirection::FromLeft:
            startGeom.moveLeft(-finalGeom.width());
            break;
        case SlideDirection::FromRight:
            startGeom.moveLeft(parent->width());
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
            endGeom.moveTop(-startGeom.height());
            break;
        case SlideDirection::FromBottom:
            endGeom.moveTop(parent->height());
            break;
        case SlideDirection::FromLeft:
            endGeom.moveLeft(-startGeom.width());
            break;
        case SlideDirection::FromRight:
            endGeom.moveLeft(parent->width());
            break;
    }

    auto anim = new QPropertyAnimation(widget, "geometry");
    anim->setDuration(duration);
    anim->setStartValue(startGeom);
    anim->setEndValue(endGeom);
    anim->setEasingCurve(QEasingCurve::InCubic);
    return anim;
}

POPUPUI_END_NAMESPACE
