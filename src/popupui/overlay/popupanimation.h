#pragma once

#include <qwidget.h>
#include <qabstractanimation.h>
#include <popupui/comm/global.h>

POPUPUI_BEGIN_NAMESPACE

class POPUPUI_EXPORT PopupAnimation {
public:
    explicit PopupAnimation(int duration = 300): duration(duration) {}
    virtual ~PopupAnimation() = default;

    virtual QAbstractAnimation* enter(QWidget* widget) = 0;
    virtual QAbstractAnimation* exit(QWidget* widget) = 0;

protected:
    int duration;

    friend class DialogOverlay;
    friend class ParallelPopupAnimation;
};

POPUPUI_END_NAMESPACE