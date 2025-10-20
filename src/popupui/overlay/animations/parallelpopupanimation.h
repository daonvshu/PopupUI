#pragma once

#include <qobject.h>

#include "../popupanimation.h"

POPUPUI_BEGIN_NAMESPACE

class ParallelPopupAnimation : public PopupAnimation {
public:
    explicit ParallelPopupAnimation(const QList<PopupAnimation*>& animations);
    ~ParallelPopupAnimation();

    QAbstractAnimation* enter(QWidget* widget) override;
    QAbstractAnimation* exit(QWidget* widget) override;

private:
    QList<PopupAnimation*> animations;
};

POPUPUI_END_NAMESPACE