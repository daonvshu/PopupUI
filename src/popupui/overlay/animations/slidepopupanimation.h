#pragma once

#include <qobject.h>

#include "../popupanimation.h"

POPUPUI_BEGIN_NAMESPACE

enum class SlideDirection {
    FromTop,
    FromBottom,
    FromLeft,
    FromRight
};

class POPUPUI_EXPORT SlidePopupAnimation : public PopupAnimation {
public:
    explicit SlidePopupAnimation(SlideDirection dir = SlideDirection::FromBottom, int duration = 300);

    QAbstractAnimation* enter(QWidget* widget) override;
    QAbstractAnimation* exit(QWidget* widget) override;

private:
    SlideDirection dir;
};

POPUPUI_END_NAMESPACE