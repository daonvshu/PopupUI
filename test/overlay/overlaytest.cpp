#include "overlaytest.h"
#include "overlaydlg1.h"

#include <popupui/overlay/dialogoverlay.h>
#include <popupui/overlay/animations/fadepopupanimation.h>
#include <qdebug.h>

using namespace PopupUI;
OverlayTest::OverlayTest(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    DialogOverlay::registerHostWindow(this);
    //DialogOverlay::enableMaskAnimation(false);
    DialogOverlay::setFadeAnimationDefault(150);
    //DialogOverlay::setScaleAnimationDefault(1500);
    //DialogOverlay::setSlideAnimationDefault(1500);
    //DialogOverlay::enablePopupAnimation(false);
}

void OverlayTest::on_btn_show_clicked() {
    auto dlg = new OverlayDlg1;
    //DialogOverlay::showDialog(dlg, new FadePopupAnimation);
    DialogOverlay::showDialog(dlg,
        PopupAnimationType::Fade | PopupAnimationType::Slide,
        PopupProperty()
                //.showBaseMaskLayer(false)
        );
}

void OverlayTest::on_btn_create_dynamic_host_clicked() {
    if (dynamicHost) {
        dynamicHost->raise();
        dynamicHost->activateWindow();
        return;
    }

    dynamicHost = new QWidget;
    dynamicHost->resize(420, 280);
    dynamicHost->setWindowTitle("Dynamic DialogOverlay Host");
    DialogOverlay::registerHostWindow(dynamicHost, QStringLiteral("dynamic-host"));
    dynamicHost->show();
    ui.btn_create_dynamic_host->setEnabled(false);
}

void OverlayTest::on_btn_show_dynamic_host_clicked() {
    if (!DialogOverlay::isHostWindowAvailable(QStringLiteral("dynamic-host"))) {
        qDebug() << "Dynamic host has been destroyed; dialog is not attached.";
        return;
    }

    auto dlg = new OverlayDlg1;
    DialogOverlay::showDialog(dlg, PopupAnimationType::Fade,
        PopupProperty(QStringLiteral("dynamic-host")));
}

void OverlayTest::on_btn_destroy_dynamic_host_clicked() {
    if (!dynamicHost) {
        return;
    }

    delete dynamicHost;
    Q_ASSERT(!dynamicHost);
    Q_ASSERT(!DialogOverlay::isHostWindowAvailable(QStringLiteral("dynamic-host")));
}
