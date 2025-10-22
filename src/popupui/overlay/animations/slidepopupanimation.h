#pragma once

#include <qobject.h>

#include "../popupanimation.h"
#include "../slidedirection.h"

POPUPUI_BEGIN_NAMESPACE

struct POPUPUI_EXPORT SlidePopupAnimationParams : PopupAnimationParams {
    SlideDirection dir = SlideDirection::FromBottom;
};

class POPUPUI_EXPORT SlidePopupAnimation : public PopupAnimation {
public:
    explicit SlidePopupAnimation(SlideDirection dir = SlideDirection::FromBottom, int duration = 300);

    QAbstractAnimation* enter(QWidget* widget) override;
    QAbstractAnimation* exit(QWidget* widget) override;

    void setParams(PopupAnimationParams* params) override;

private:
    SlideDirection dir;
};

POPUPUI_END_NAMESPACE