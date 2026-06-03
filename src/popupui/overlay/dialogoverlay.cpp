#include "dialogoverlay.h"

#include <qpointer.h>
#include <qstack.h>
#include <qpropertyanimation.h>
#include <qeventloop.h>
#include <qtimer.h>

#include "animations/fadepopupanimation.h"
#include "animations/parallelpopupanimation.h"
#include "animations/scalepopupanimation.h"
#include "animations/slidepopupanimation.h"

POPUPUI_BEGIN_NAMESPACE

struct DialogLayer {
    QPointer<QWidget> dialog;
    QPointer<QWidget> dlgMask;
    PopupAnimation* popupAnim;
    QEventLoop* loop;
    bool closing;
    PopupProperty prop;

    DialogLayer()
        : dialog(nullptr)
        , dlgMask(nullptr)
        , popupAnim(nullptr)
        , loop(nullptr)
        , closing(false)
    {}

    DialogLayer(QWidget* dialog, QWidget* dlgMask, PopupAnimation* popupAnim, QEventLoop* loop, const PopupProperty& prop)
        : dialog(dialog), dlgMask(dlgMask), popupAnim(popupAnim), loop(loop), closing(false), prop(prop)
    {}
};

struct DialogOverlayData {
    QWidget* hostWindow = nullptr;
    QWidget* overlayMask = nullptr;
    QStack<DialogLayer*> stack;
    bool baseMaskAnimationEnabled = true;
    bool popupAnimationEnabled = true;

    PopupAnimationParams fadeDefaultParams;
    ScalePopupAnimationParams scaleDefaultParams;
    SlidePopupAnimationParams slideDefaultParams;
};

DialogOverlayData DialogOverlay::d;

static void finalizeDialogClose(const QPointer<QWidget>& dlg) {
    if (!dlg) {
        return;
    }
    dlg->hide();
    QTimer::singleShot(0, dlg, &QWidget::close);
}

void DialogOverlay::registerHostWindow(QWidget* host, const QColor& backgroundMaskColor) {
    d.hostWindow = host;
    d.overlayMask = new DialogMask(host, nullptr, PopupProperty().setMaskColor(backgroundMaskColor));
    d.overlayMask->raise();
    d.overlayMask->setVisible(false);
}

void DialogOverlay::enableMaskAnimation(bool enable) {
    d.baseMaskAnimationEnabled = enable;
}

void DialogOverlay::enablePopupAnimation(bool enable) {
    d.popupAnimationEnabled = enable;
}

void DialogOverlay::setFadeAnimationDefault(int duration) {
    d.fadeDefaultParams.duration = duration;
}

void DialogOverlay::setScaleAnimationDefault(int duration, qreal scaleFactor) {
    d.scaleDefaultParams.duration = duration;
    d.scaleDefaultParams.scaleFactor = scaleFactor;
}

void DialogOverlay::setSlideAnimationDefault(int duration, int fromOffset, SlideDirection direction) {
    d.slideDefaultParams.duration = duration;
    d.slideDefaultParams.dir = direction;
    d.slideDefaultParams.fromOffset = fromOffset;
}

bool DialogOverlay::shouldPlayAnimation(const PopupAnimation* popupAnim) {
    return d.popupAnimationEnabled && popupAnim && popupAnim->duration > 0;
}

void DialogOverlay::showDialog(QWidget* dlg, PopupAnimation* popupAnim, const PopupProperty& prop) {
    if (d.hostWindow == nullptr) {
        qFatal("Call DialogOverlay::registerHostWindow before show!");
    }
    if (d.overlayMask->isHidden()) {
        if (prop.baseMaskVisible) {
            d.overlayMask->setVisible(true);
            if (d.baseMaskAnimationEnabled && shouldPlayAnimation(popupAnim)) {
                FadePopupAnimation maskAnim(popupAnim->duration);
                maskAnim.enter(d.overlayMask)->start(QPropertyAnimation::DeleteWhenStopped);
            }
        }
    }
    dlg->setParent(d.hostWindow);

    auto mask = new DialogMask(d.hostWindow, dlg, prop);
    mask->show();
    dlg->show();
    dlg->raise();

    if (shouldPlayAnimation(popupAnim)) {
        popupAnim->enter(dlg)->start(QPropertyAnimation::DeleteWhenStopped);
        FadePopupAnimation maskAnim(popupAnim->duration);
        maskAnim.enter(mask)->start(QPropertyAnimation::DeleteWhenStopped);
    }

    auto layer = new DialogLayer(dlg, mask, popupAnim, nullptr, prop);
    d.stack.push(layer);
    bindCloseEvent(layer);
}

void DialogOverlay::showDialog(QWidget* dlg, PopupAnimationTypes types, const PopupProperty& prop) {
    showDialog(dlg, getAnimation(types), prop);
}

void DialogOverlay::showDialogExec(QWidget& dlg, PopupAnimation* popupAnim, const PopupProperty& prop) {
    auto showProp = prop;
    showProp.deleteOnClose = false;
    QEventLoop loop;
    showDialog(&dlg, popupAnim, showProp);
    d.stack.top()->loop = &loop;
    loop.exec();
}

void DialogOverlay::showDialogExec(QWidget& dlg, PopupAnimationTypes types, const PopupProperty& prop) {
    showDialogExec(dlg, getAnimation(types), prop);
}

void DialogOverlay::closeTopDialog() {
    if (d.stack.isEmpty()) return;
    auto layer = d.stack.pop();
    closeTarget(layer);
    if (d.stack.isEmpty()) {
        d.overlayMask->setVisible(false);
        d.overlayMask->setWindowOpacity(1);
    }
}

void DialogOverlay::closeAll() {
    if (d.stack.isEmpty()) {
        return;
    }
    while (!d.stack.isEmpty()) {
        auto layer = d.stack.pop();
        closeTarget(layer);
    }
    if (d.overlayMask) {
        d.overlayMask->setVisible(false);
        d.overlayMask->setWindowOpacity(1);
    }
}

void DialogOverlay::bindCloseEvent(DialogLayer* layer) {
    auto mask = layer->dlgMask;
    if (!mask) {
        return;
    }
    auto maskWidget = qobject_cast<DialogMask*>(mask.data());
    if (!maskWidget) {
        return;
    }
    QObject::connect(maskWidget, &DialogMask::requestClose, maskWidget, [layer] {
        if (layer->closing) {
            return;
        }
        auto popupAnim = layer->popupAnim;
        const auto animDuration = shouldPlayAnimation(popupAnim) ? popupAnim->duration : 0;
        const auto shouldAnimateBaseMask = d.baseMaskAnimationEnabled && animDuration > 0;
        auto dlg = layer->dialog;
        for (int i = d.stack.size() - 1; i >= 0; --i) {
            if (d.stack[i]->dialog == dlg) {
                closeTarget(layer);
                d.stack.remove(i);
                break;
            }
        }
        if (d.stack.isEmpty() || !d.stack.last()->prop.baseMaskVisible) {
            if (shouldAnimateBaseMask) {
                FadePopupAnimation maskAnim(animDuration);
                auto anim = maskAnim.exit(d.overlayMask);
                if (anim) {
                    QObject::connect(anim, &QAbstractAnimation::finished, [] {
                        d.overlayMask->setVisible(false);
                        d.overlayMask->setWindowOpacity(1);
                    });
                    anim->start(QPropertyAnimation::DeleteWhenStopped);
                    return;
                }
            }
            d.overlayMask->setVisible(false);
        }
    });
}

void DialogOverlay::closeTarget(DialogLayer* layer) {
    if (!layer || layer->closing) {
        return;
    }
    layer->closing = true;

    const auto mask = layer->dlgMask;
    const auto dlg = layer->dialog;
    auto popupAnim = layer->popupAnim;
    if (shouldPlayAnimation(popupAnim)) {
        FadePopupAnimation maskAnim(popupAnim->duration);
        auto anim = mask ? maskAnim.exit(mask) : nullptr;
        if (anim) {
            QObject::connect(anim, &QAbstractAnimation::finished, [mask] {
                if (!mask) {
                    return;
                }
                static_cast<DialogMask*>(mask.data())->unbindEvent();
                mask->deleteLater();
            });
        } else if (mask) {
            static_cast<DialogMask*>(mask.data())->unbindEvent();
            mask->deleteLater();
        }
        if (anim) anim->start(QPropertyAnimation::DeleteWhenStopped);

        anim = dlg ? popupAnim->exit(dlg) : nullptr;
        if (anim) {
            QObject::connect(anim, &QAbstractAnimation::finished, [layer] {
                finalizeDialogClose(layer->dialog);
                delete layer->popupAnim;
                if (layer->loop) {
                    layer->loop->quit();
                }
                delete layer;
            });
            anim->start(QPropertyAnimation::DeleteWhenStopped);
        } else {
            finalizeDialogClose(dlg);
            delete popupAnim;
            if (layer->loop) {
                layer->loop->quit();
            }
            delete layer;
        }
    } else {
        if (mask) {
            static_cast<DialogMask*>(mask.data())->unbindEvent();
        }
        finalizeDialogClose(dlg);
        if (mask) {
            mask->deleteLater();
        }
        delete popupAnim;
        if (layer->loop) {
            layer->loop->quit();
        }
        delete layer;
    }
}

PopupAnimation* DialogOverlay::getAnimation(PopupAnimationTypes types) {
    QList<PopupAnimation*> animations;
    if (types.testFlag(PopupAnimationType::Fade)) {
        animations << new FadePopupAnimation(d.fadeDefaultParams.duration);
    }
    if (types.testFlag(PopupAnimationType::Scale)) {
        animations << new ScalePopupAnimation(d.scaleDefaultParams.scaleFactor, d.scaleDefaultParams.duration);
    }
    if (types.testFlag(PopupAnimationType::Slide)) {
        animations << new SlidePopupAnimation(d.slideDefaultParams.dir, d.slideDefaultParams.fromOffset, d.slideDefaultParams.duration);
    }
    if (animations.isEmpty()) {
        return nullptr;
    }
    return animations.size() == 1 ? animations.first() : new ParallelPopupAnimation(animations);
}

POPUPUI_END_NAMESPACE
