#include "dialogoverlay.h"

#include <qstack.h>
#include <qpropertyanimation.h>
#include <qeventloop.h>

#include "animations/fadepopupanimation.h"
#include "animations/parallelpopupanimation.h"
#include "animations/scalepopupanimation.h"
#include "animations/slidepopupanimation.h"

POPUPUI_BEGIN_NAMESPACE

struct DialogLayer {
    QWidget* dialog;
    QWidget* dlgMask;
    PopupAnimation* popupAnim;
    QEventLoop* loop;

    DialogLayer()
        : dialog(nullptr)
        , dlgMask(nullptr)
        , popupAnim(nullptr)
        , loop(nullptr)
    {}

    DialogLayer(QWidget* dialog, QWidget* dlgMask, PopupAnimation* popupAnim, QEventLoop* loop)
        : dialog(dialog), dlgMask(dlgMask), popupAnim(popupAnim), loop(loop)
    {}
};

struct DialogOverlayData {
    QWidget* hostWindow = nullptr;
    QWidget* overlayMask = nullptr;
    QStack<DialogLayer*> stack;
    bool baseMaskAnimationEnabled = true;

    PopupAnimationParams fadeDefaultParams;
    ScalePopupAnimationParams scaleDefaultParams;
    SlidePopupAnimationParams slideDefaultParams;
};

DialogOverlayData DialogOverlay::d;

void DialogOverlay::registerHostWindow(QWidget* host, const QColor& backgroundMaskColor) {
    d.hostWindow = host;
    d.overlayMask = new DialogMask(host, nullptr, PopupProperty().setMaskColor(backgroundMaskColor));
    d.overlayMask->raise();
    d.overlayMask->setVisible(false);
}

void DialogOverlay::enableMaskAnimation(bool enable) {
    d.baseMaskAnimationEnabled = enable;
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

void DialogOverlay::showDialog(QWidget* dlg, PopupAnimation* popupAnim, const PopupProperty& prop) {
    if (d.hostWindow == nullptr) {
        qFatal("Call DialogOverlay::registerHostWindow before show!");
    }
    if (d.overlayMask->isHidden()) {
        if (d.stack.isEmpty()) {
            if (prop.baseMaskVisible) {
                d.overlayMask->setVisible(true);
                if (d.baseMaskAnimationEnabled && popupAnim) {
                    FadePopupAnimation maskAnim(popupAnim->duration);
                    maskAnim.enter(d.overlayMask)->start(QPropertyAnimation::DeleteWhenStopped);
                }
            }
        }
    }
    dlg->setParent(d.hostWindow);

    auto mask = new DialogMask(d.hostWindow, dlg, prop);
    mask->show();
    dlg->show();
    dlg->raise();

    if (popupAnim) {
        popupAnim->enter(dlg)->start(QPropertyAnimation::DeleteWhenStopped);
        FadePopupAnimation maskAnim(popupAnim->duration);
        maskAnim.enter(mask)->start(QPropertyAnimation::DeleteWhenStopped);
    }

    auto layer = new DialogLayer(dlg, mask, popupAnim, nullptr);
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

void DialogOverlay::bindCloseEvent(const DialogLayer* layer) {
    auto mask = layer->dlgMask;
    QObject::connect(static_cast<DialogMask*>(mask), &DialogMask::requestClose, mask, [layer] {
        auto popupAnim = layer->popupAnim;
        auto dlg = layer->dialog;
        for (int i = d.stack.size() - 1; i >= 0; --i) {
            if (d.stack[i]->dialog == dlg) {
                closeTarget(layer);
                d.stack.remove(i);
                break;
            }
        }
        if (d.stack.isEmpty()) {
            if (d.baseMaskAnimationEnabled && popupAnim) {
                FadePopupAnimation maskAnim(popupAnim->duration);
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

void DialogOverlay::closeTarget(const DialogLayer* layer) {
    const auto mask = layer->dlgMask;
    const auto dlg = layer->dialog;
    auto popupAnim = layer->popupAnim;
    if (popupAnim) {
        FadePopupAnimation maskAnim(popupAnim->duration);
        auto anim = maskAnim.exit(mask);
        QObject::connect(anim, &QAbstractAnimation::finished, [mask] {
            static_cast<DialogMask*>(mask)->unbindEvent();
            mask->deleteLater();
        });
        if (anim) anim->start(QPropertyAnimation::DeleteWhenStopped);

        anim = popupAnim->exit(dlg);
        QObject::connect(anim, &QAbstractAnimation::finished, [layer] {
            layer->dialog->close();
            delete layer->popupAnim;
            if (layer->loop) {
                layer->loop->quit();
            }
            delete layer;
        });
        if (anim) anim->start(QPropertyAnimation::DeleteWhenStopped);
    } else {
        dlg->close();
        mask->deleteLater();
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
