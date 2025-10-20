#include "dialogoverlay.h"

#include <qstack.h>
#include <qpropertyanimation.h>

#include "animations/fadepopupanimation.h"

POPUPUI_BEGIN_NAMESPACE
    struct DialogOverlayData {
    QWidget* hostWindow = nullptr;
    QWidget* overlayMask = nullptr;
    QStack<DialogOverlay::Layer> stack;
    bool baseMaskAnimationEnabled = true;

    void closeTop() {
        auto layer = stack.pop();
        layer.dialog->close();
        layer.dlgMask->deleteLater();
    }
};

DialogOverlayData DialogOverlay::d;

void DialogOverlay::registerHostWindow(QWidget* host, const QColor& backgroundMaskColor) {
    d.hostWindow = host;
    d.overlayMask = new DialogMask(host, nullptr, DialogMask::Property().setMaskColor(backgroundMaskColor));
    d.overlayMask->raise();
    d.overlayMask->setVisible(false);
}

void DialogOverlay::enableMaskAnimation(bool enable) {
    d.baseMaskAnimationEnabled = enable;
}

void DialogOverlay::showDialog(QWidget* dlg, PopupAnimation* popupAnim, const DialogMask::Property& prop) {
    if (d.hostWindow == nullptr) {
        qFatal("Call DialogOverlay::registerHostWindow before show!");
    }
    if (d.overlayMask->isHidden()) {
        d.overlayMask->setVisible(true);
        if (d.baseMaskAnimationEnabled && popupAnim) {
            FadePopupAnimation maskAnim(popupAnim->duration);
            maskAnim.enter(d.overlayMask)->start(QPropertyAnimation::DeleteWhenStopped);
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

    Layer layer{ dlg, mask };
    bindCloseEvent(layer, popupAnim);
    d.stack.push(layer);
}

void DialogOverlay::closeTopDialog() {
    if (d.stack.isEmpty()) return;
    d.closeTop();
    if (d.stack.isEmpty()) {
        d.overlayMask->setVisible(false);
    }
}

void DialogOverlay::clear() {
    while (!d.stack.isEmpty()) {
        d.closeTop();
    }
    if (d.overlayMask) {
        d.overlayMask->setVisible(false);
    }
}

void DialogOverlay::bindCloseEvent(const Layer& layer, PopupAnimation* popupAnim) {
    auto mask = layer.dlgMask;
    auto dlg = layer.dialog;
    QObject::connect(static_cast<DialogMask*>(mask), &DialogMask::requestClose, mask, [=] {
        for (int i = d.stack.size() - 1; i >= 0; --i) {
            if (d.stack[i].dialog == dlg) {
                if (popupAnim) {
                    FadePopupAnimation maskAnim(popupAnim->duration);
                    auto anim = maskAnim.exit(mask);
                    QObject::connect(anim, &QAbstractAnimation::finished, [=] {
                        mask->deleteLater();
                    });
                    if (anim) anim->start(QPropertyAnimation::DeleteWhenStopped);

                    anim = popupAnim->exit(dlg);
                    QObject::connect(anim, &QAbstractAnimation::finished, [=] {
                        dlg->close();
                        delete popupAnim;
                    });
                    if (anim) anim->start(QPropertyAnimation::DeleteWhenStopped);
                } else {
                    dlg->close();
                    mask->deleteLater();
                }
                d.stack.remove(i);
                break;
            }
        }
        if (d.stack.isEmpty()) {
            if (d.baseMaskAnimationEnabled && popupAnim) {
                FadePopupAnimation maskAnim(popupAnim->duration);
                auto anim = maskAnim.exit(d.overlayMask);
                if (anim) {
                    QObject::connect(anim, &QAbstractAnimation::finished, [=] {
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

POPUPUI_END_NAMESPACE
