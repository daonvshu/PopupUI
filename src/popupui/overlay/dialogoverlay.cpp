#include "dialogoverlay.h"

#include <qpointer.h>
#include <qhash.h>
#include <qstack.h>
#include <qpropertyanimation.h>
#include <qeventloop.h>
#include <qtimer.h>

#include "popupui/animations/fadepopupanimation.h"
#include "popupui/animations/parallelpopupanimation.h"
#include "popupui/animations/scalepopupanimation.h"
#include "popupui/animations/slidepopupanimation.h"

POPUPUI_BEGIN_NAMESPACE

struct DialogHostWindow;

struct DialogLayer {
    QPointer<QWidget> dialog;
    QPointer<QWidget> dlgMask;
    PopupAnimation* popupAnim;
    QEventLoop* loop;
    DialogHostWindow* hostWindow;
    bool closing;
    PopupProperty prop;

    DialogLayer()
        : dialog(nullptr)
        , dlgMask(nullptr)
        , popupAnim(nullptr)
        , loop(nullptr)
        , hostWindow(nullptr)
        , closing(false)
    {}

    DialogLayer(QWidget* dialog, QWidget* dlgMask, PopupAnimation* popupAnim, QEventLoop* loop, DialogHostWindow* hostWindow, const PopupProperty& prop)
        : dialog(dialog), dlgMask(dlgMask), popupAnim(popupAnim), loop(loop), hostWindow(hostWindow), closing(false), prop(prop)
    {}
};

struct DialogHostWindow {
    QPointer<QWidget> hostWindow;
    QPointer<QWidget> overlayMask;
    QStack<DialogLayer*> stack;
};

struct DialogOverlayData {
    QHash<QString, DialogHostWindow*> hostWindows;
    QString defaultHostId;
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
    registerHostWindow(host, QString(), backgroundMaskColor);
}

void DialogOverlay::registerHostWindow(QWidget* host, const QString& hostId, const QColor& backgroundMaskColor) {
    if (!host) {
        qFatal("DialogOverlay host window cannot be null!");
    }
    auto previousHostWindow = d.hostWindows.value(hostId, nullptr);
    if (previousHostWindow) {
        closeAll(hostId);
        delete previousHostWindow;
    }

    auto hostWindow = new DialogHostWindow;
    hostWindow->hostWindow = host;
    hostWindow->overlayMask = new DialogMask(host, nullptr, PopupProperty().setMaskColor(backgroundMaskColor));
    hostWindow->overlayMask->raise();
    hostWindow->overlayMask->setVisible(false);
    if (d.hostWindows.isEmpty()) {
        d.defaultHostId = hostId;
    }
    d.hostWindows.insert(hostId, hostWindow);
}

bool DialogOverlay::isHostWindowAvailable(const QString& hostId) {
    const auto selectedHostId = hostId.isEmpty() ? d.defaultHostId : hostId;
    const auto hostWindow = d.hostWindows.value(selectedHostId, nullptr);
    return hostWindow && hostWindow->hostWindow && hostWindow->overlayMask;
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
    const auto selectedHostId = prop.hostWindowId.isEmpty() ? d.defaultHostId : prop.hostWindowId;
    auto hostWindow = d.hostWindows.value(selectedHostId, nullptr);
    if (!hostWindow || !hostWindow->hostWindow || !hostWindow->overlayMask) {
        qFatal("Call DialogOverlay::registerHostWindow before show!");
    }
    if (hostWindow->overlayMask->isHidden()) {
        if (prop.baseMaskVisible) {
            hostWindow->overlayMask->setVisible(true);
            if (d.baseMaskAnimationEnabled && shouldPlayAnimation(popupAnim)) {
                FadePopupAnimation maskAnim(popupAnim->duration);
                maskAnim.enter(hostWindow->overlayMask)->start(QPropertyAnimation::DeleteWhenStopped);
            }
        }
    }
    dlg->setParent(hostWindow->hostWindow);

    auto mask = new DialogMask(hostWindow->hostWindow, dlg, prop);
    mask->show();
    dlg->show();
    dlg->raise();

    if (shouldPlayAnimation(popupAnim)) {
        popupAnim->enter(dlg)->start(QPropertyAnimation::DeleteWhenStopped);
        FadePopupAnimation maskAnim(popupAnim->duration);
        maskAnim.enter(mask)->start(QPropertyAnimation::DeleteWhenStopped);
    }

    auto layer = new DialogLayer(dlg, mask, popupAnim, nullptr, hostWindow, prop);
    hostWindow->stack.push(layer);
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
    const auto selectedHostId = showProp.hostWindowId.isEmpty() ? d.defaultHostId : showProp.hostWindowId;
    d.hostWindows.value(selectedHostId)->stack.top()->loop = &loop;
    loop.exec();
}

void DialogOverlay::showDialogExec(QWidget& dlg, PopupAnimationTypes types, const PopupProperty& prop) {
    showDialogExec(dlg, getAnimation(types), prop);
}

void DialogOverlay::closeTopDialog(const QString& hostId) {
    const auto selectedHostId = hostId.isEmpty() ? d.defaultHostId : hostId;
    auto hostWindow = d.hostWindows.value(selectedHostId, nullptr);
    if (!hostWindow || hostWindow->stack.isEmpty()) return;
    auto layer = hostWindow->stack.pop();
    closeTarget(layer);
    if (hostWindow->stack.isEmpty() && hostWindow->overlayMask) {
        hostWindow->overlayMask->setVisible(false);
        hostWindow->overlayMask->setWindowOpacity(1);
    }
}

void DialogOverlay::closeAll(const QString& hostId) {
    const auto selectedHostId = hostId.isEmpty() ? d.defaultHostId : hostId;
    auto hostWindow = d.hostWindows.value(selectedHostId, nullptr);
    if (!hostWindow || hostWindow->stack.isEmpty()) {
        return;
    }
    while (!hostWindow->stack.isEmpty()) {
        auto layer = hostWindow->stack.pop();
        closeTarget(layer);
    }
    if (hostWindow->overlayMask) {
        hostWindow->overlayMask->setVisible(false);
        hostWindow->overlayMask->setWindowOpacity(1);
    }
}

void DialogOverlay::bindCloseEvent(DialogLayer* layer) {
    auto hostWindow = layer->hostWindow;
    auto mask = layer->dlgMask;
    if (!hostWindow || !mask) {
        return;
    }
    auto maskWidget = qobject_cast<DialogMask*>(mask.data());
    if (!maskWidget) {
        return;
    }
    QObject::connect(maskWidget, &DialogMask::requestClose, maskWidget, [layer, hostWindow] {
        if (layer->closing) {
            return;
        }
        auto popupAnim = layer->popupAnim;
        const auto animDuration = shouldPlayAnimation(popupAnim) ? popupAnim->duration : 0;
        const auto shouldAnimateBaseMask = d.baseMaskAnimationEnabled && animDuration > 0;
        auto dlg = layer->dialog;
        for (int i = hostWindow->stack.size() - 1; i >= 0; --i) {
            if (hostWindow->stack[i]->dialog == dlg) {
                closeTarget(layer);
                hostWindow->stack.remove(i);
                break;
            }
        }
        if (hostWindow->stack.isEmpty() || !hostWindow->stack.last()->prop.baseMaskVisible) {
            if (shouldAnimateBaseMask) {
                FadePopupAnimation maskAnim(animDuration);
                auto overlayMask = hostWindow->overlayMask;
                auto anim = overlayMask ? maskAnim.exit(overlayMask) : nullptr;
                if (anim) {
                    QObject::connect(anim, &QAbstractAnimation::finished, [overlayMask] {
                        if (!overlayMask) {
                            return;
                        }
                        overlayMask->setVisible(false);
                        overlayMask->setWindowOpacity(1);
                    });
                    anim->start(QPropertyAnimation::DeleteWhenStopped);
                    return;
                }
            }
            if (hostWindow->overlayMask) {
                hostWindow->overlayMask->setVisible(false);
            }
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
