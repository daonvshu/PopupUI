#pragma once

#include <qobject.h>

#include "../popupanimation.h"

POPUPUI_BEGIN_NAMESPACE

class ScalePopupAnimation : public PopupAnimation {
public:
    explicit ScalePopupAnimation(int duration = 300, qreal scaleFactor = 0.8);

    QAbstractAnimation* enter(QWidget* widget) override;
    QAbstractAnimation* exit(QWidget* widget) override;

private:
    qreal scaleFactor;
};

POPUPUI_END_NAMESPACE