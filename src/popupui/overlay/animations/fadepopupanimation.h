#pragma once

#include <qobject.h>

#include "../popupanimation.h"

POPUPUI_BEGIN_NAMESPACE

class FadePopupAnimation : public PopupAnimation {
public:
    using PopupAnimation::PopupAnimation;

    QAbstractAnimation* enter(QWidget* widget) override;
    QAbstractAnimation* exit(QWidget* widget) override;
};

POPUPUI_END_NAMESPACE