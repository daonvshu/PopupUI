#pragma once

#include <qwidget.h>
#include <popupui/comm/global.h>

#include "popupanimation.h"
#include "dialogmask.h"

POPUPUI_BEGIN_NAMESPACE

struct DialogOverlayData;
class DialogOverlay {
    Q_DISABLE_COPY(DialogOverlay)

public:
    struct Layer {
        QWidget* dialog;
        QWidget* dlgMask;
        PopupAnimation* popupAnim;
    };

    static void registerHostWindow(QWidget* host, const QColor& backgroundMaskColor = QColor(0, 0, 0, 128));
    static void enableMaskAnimation(bool enable);
    static void showDialog(QWidget* dlg, PopupAnimation* popupAnim = nullptr, const PopupProperty& prop = PopupProperty());
    static void closeTopDialog();
    static void closeAll();

public:
    DialogOverlay() = delete;
    ~DialogOverlay() = delete;

private:
    static DialogOverlayData d;

private:
    static void bindCloseEvent(const Layer& layer);
    static void closeTarget(const Layer& layer);
};

POPUPUI_END_NAMESPACE