#pragma once

#include <qobject.h>

#include "../popupanimation.h"

POPUPUI_BEGIN_NAMESPACE

struct POPUPUI_EXPORT ScalePopupAnimationParams : PopupAnimationParams {
    qreal scaleFactor = 0.8;
};

class POPUPUI_EXPORT ScalePopupAnimation : public PopupAnimation {
public:
    explicit ScalePopupAnimation(qreal scaleFactor = 0.8, int duration = 300);

    QAbstractAnimation* enter(QWidget* widget) override;
    QAbstractAnimation* exit(QWidget* widget) override;

    void setParams(PopupAnimationParams* params) override;

private:
    qreal scaleFactor;
};

POPUPUI_END_NAMESPACE