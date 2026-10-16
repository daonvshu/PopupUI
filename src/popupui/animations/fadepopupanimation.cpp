#include "fadepopupanimation.h"

#include <QGraphicsOpacityEffect>
#include <qpointer.h>
#include <qpropertyanimation.h>

POPUPUI_BEGIN_NAMESPACE

namespace {

QAbstractAnimation* createFadeAnimation(QWidget* widget, int duration, qreal startOpacity, qreal endOpacity) {
    if (!widget) {
        return nullptr;
    }

    auto* effect = new QGraphicsOpacityEffect(widget);
    effect->setOpacity(startOpacity);
    widget->setGraphicsEffect(effect);

    auto* anim = new QPropertyAnimation(effect, "opacity");
    anim->setDuration(duration);
    anim->setStartValue(startOpacity);
    anim->setEndValue(endOpacity);

    const auto removeEffect = [widget = QPointer<QWidget>(widget), effect = QPointer<QGraphicsOpacityEffect>(effect)] {
        if (widget && effect && widget->graphicsEffect() == effect.data()) {
            widget->setGraphicsEffect(nullptr);
        }
    };
    QObject::connect(anim, &QAbstractAnimation::stateChanged, widget,
        [removeEffect](QAbstractAnimation::State state, QAbstractAnimation::State) {
            if (state == QAbstractAnimation::Stopped) {
                removeEffect();
            }
        });
    QObject::connect(anim, &QObject::destroyed, widget, removeEffect);
    return anim;
}

} // namespace

QAbstractAnimation* FadePopupAnimation::enter(QWidget* widget) {
    return createFadeAnimation(widget, duration, 0.0, 1.0);
}

QAbstractAnimation* FadePopupAnimation::exit(QWidget* widget) {
    return createFadeAnimation(widget, duration, 1.0, 0.0);
}


POPUPUI_END_NAMESPACE
