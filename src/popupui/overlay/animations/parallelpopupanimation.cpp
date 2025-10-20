#include "parallelpopupanimation.h"

#include <qparallelanimationgroup.h>

POPUPUI_BEGIN_NAMESPACE

ParallelPopupAnimation::ParallelPopupAnimation(const QList<PopupAnimation*>& animations)
    : PopupAnimation([&] {
        int maxDur = 0;
        for (auto anim : animations) {
            maxDur = qMax(maxDur, anim->duration);
        }
        return maxDur;
    } ())
    , animations(animations)
{}

ParallelPopupAnimation::~ParallelPopupAnimation() {
    qDeleteAll(animations);
}

QAbstractAnimation* ParallelPopupAnimation::enter(QWidget* widget) {
    auto *group = new QParallelAnimationGroup(widget);
    for (auto anim : animations) {
        group->addAnimation(anim->enter(widget));
    }
    return group;
}

QAbstractAnimation* ParallelPopupAnimation::exit(QWidget* widget) {
    auto *group = new QParallelAnimationGroup(widget);
    for (auto anim : animations) {
        group->addAnimation(anim->exit(widget));
    }
    return group;
}

POPUPUI_END_NAMESPACE
