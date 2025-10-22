#pragma once

#include <qwidget.h>
#include <popupui/comm/global.h>

#include "popupanimation.h"
#include "dialogmask.h"
#include "slidedirection.h"

POPUPUI_BEGIN_NAMESPACE

struct DialogOverlayData;
struct DialogLayer;
class POPUPUI_EXPORT DialogOverlay {
    Q_DISABLE_COPY(DialogOverlay)

public:
    static void registerHostWindow(QWidget* host, const QColor& backgroundMaskColor = QColor(0, 0, 0, 128));
    static void enableMaskAnimation(bool enable);
    static void setFadeAnimationDefault(int duration);
    static void setScaleAnimationDefault(int duration, qreal scaleFactor = 0.8);
    static void setSlideAnimationDefault(int duration, SlideDirection direction = SlideDirection::FromBottom);

    static void showDialog(QWidget* dlg, PopupAnimation* popupAnim = nullptr, const PopupProperty& prop = PopupProperty());
    static void showDialog(QWidget* dlg, PopupAnimationTypes types = PopupAnimationType::None, const PopupProperty& prop = PopupProperty());
    static void showDialogExec(QWidget& dlg, PopupAnimation* popupAnim = nullptr, const PopupProperty& prop = PopupProperty());
    static void showDialogExec(QWidget& dlg, PopupAnimationTypes types = PopupAnimationType::None, const PopupProperty& prop = PopupProperty());

    static void closeTopDialog();
    static void closeAll();

public:
    DialogOverlay() = delete;
    ~DialogOverlay() = delete;

private:
    static DialogOverlayData d;

private:
    static void bindCloseEvent(const DialogLayer* layer);
    static void closeTarget(const DialogLayer* layer);
    static PopupAnimation* getAnimation(PopupAnimationTypes types);
};

POPUPUI_END_NAMESPACE