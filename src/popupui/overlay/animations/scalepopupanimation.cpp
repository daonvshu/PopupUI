#include "scalepopupanimation.h"

#include <qpropertyanimation.h>

POPUPUI_BEGIN_NAMESPACE

ScalePopupAnimation::ScalePopupAnimation(int duration, qreal scaleFactor)
    : PopupAnimation(duration)
    , scaleFactor(scaleFactor)
{}

QAbstractAnimation* ScalePopupAnimation::enter(QWidget* widget) {
    QRect finalGeom = widget->geometry();
    QSize finalSize = finalGeom.size();
    QPoint finalPos = finalGeom.topLeft();

    QSize startSize(finalSize.width() * scaleFactor,
                    finalSize.height() * scaleFactor);
    QPoint startPos(finalPos.x() + (finalSize.width() - startSize.width()) / 2,
                    finalPos.y() + (finalSize.height() - startSize.height()) / 2);

    QRect startGeom(startPos, startSize);

    widget->setGeometry(startGeom);

    auto anim = new QPropertyAnimation(widget, "geometry");
    anim->setDuration(duration);
    anim->setStartValue(startGeom);
    anim->setEndValue(finalGeom);
    anim->setEasingCurve(QEasingCurve::OutBack); // 后弹效果
    return anim;
}

QAbstractAnimation* ScalePopupAnimation::exit(QWidget* widget) {
    QRect startGeom = widget->geometry();
    QSize startSize = startGeom.size();
    QPoint startPos = startGeom.topLeft();

    QSize endSize(startSize.width() * scaleFactor,
                  startSize.height() * scaleFactor);
    QPoint endPos(startPos.x() + (startSize.width() - endSize.width()) / 2,
                  startPos.y() + (startSize.height() - endSize.height()) / 2);

    QRect endGeom(endPos, endSize);

    auto anim = new QPropertyAnimation(widget, "geometry");
    anim->setDuration(duration);
    anim->setStartValue(startGeom);
    anim->setEndValue(endGeom);
    anim->setEasingCurve(QEasingCurve::InBack);
    return anim;
}

POPUPUI_END_NAMESPACE
