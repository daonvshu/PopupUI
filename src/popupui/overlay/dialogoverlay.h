#pragma once

#include <qwidget.h>
#include <popupui/comm/global.h>

#include "popupanimation.h"
#include "dialogmask.h"

POPUPUI_BEGIN_NAMESPACE

struct DialogOverlayData;
class DialogOverlay {
    Q_DISABLE_COPY_MOVE(DialogOverlay)

public:
    struct Layer {
        QWidget* dialog;
        QWidget* dlgMask;
    };

    static void registerHostWindow(QWidget* host, const QColor& backgroundMaskColor = QColor(0, 0, 0, 128));
    static void enableMaskAnimation(bool enable);
    static void showDialog(QWidget* dlg, PopupAnimation* anim = nullptr, const DialogMask::Property& popupAnim = DialogMask::Property());
    static void closeTopDialog();
    static void clear();

public:
    DialogOverlay() = delete;
    ~DialogOverlay() = delete;

private:
    static DialogOverlayData d;

private:
    static void bindCloseEvent(const Layer& layer, PopupAnimation* popupAnim);
};

POPUPUI_END_NAMESPACE