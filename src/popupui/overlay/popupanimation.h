#pragma once

#include <qwidget.h>
#include <qabstractanimation.h>
#include <popupui/comm/global.h>

POPUPUI_BEGIN_NAMESPACE

struct POPUPUI_EXPORT PopupAnimationParams {
    int duration = 300;

    virtual ~PopupAnimationParams() = default;
};

class POPUPUI_EXPORT PopupAnimation {
public:
    explicit PopupAnimation(int duration = -1): duration(duration) {}
    virtual ~PopupAnimation() = default;

    virtual QAbstractAnimation* enter(QWidget* widget) = 0;
    virtual QAbstractAnimation* exit(QWidget* widget) = 0;

    virtual void setParams(PopupAnimationParams* params) {
        duration = params->duration;
    }

protected:
    int duration;

    friend class DialogOverlay;
    friend class ParallelPopupAnimation;
};

enum class PopupAnimationType {
    None = 0,
    Fade = 1,
    Slide = 2,
    Scale = 4,
};
Q_DECLARE_FLAGS(PopupAnimationTypes, PopupAnimationType)
Q_DECLARE_OPERATORS_FOR_FLAGS(PopupAnimationTypes)

POPUPUI_END_NAMESPACE