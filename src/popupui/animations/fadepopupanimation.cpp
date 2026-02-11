#include "fadepopupanimation.h"

#include <QGraphicsOpacityEffect>
#include <qpropertyanimation.h>

POPUPUI_BEGIN_NAMESPACE

QAbstractAnimation* FadePopupAnimation::enter(QWidget* widget) {
    auto* effect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(effect);

    auto* anim = new QPropertyAnimation(effect, "opacity");
    anim->setDuration(duration);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    return anim;
}

QAbstractAnimation* FadePopupAnimation::exit(QWidget* widget) {
    if (auto *effect = qobject_cast<QGraphicsOpacityEffect*>(widget->graphicsEffect())) {
        auto* anim = new QPropertyAnimation(effect, "opacity");
        anim->setDuration(duration);
        anim->setStartValue(1.0);
        anim->setEndValue(0.0);
        return anim;
    }
    return nullptr;
}


POPUPUI_END_NAMESPACE