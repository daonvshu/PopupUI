#include "overlaytest.h"
#include "overlaydlg1.h"

#include <popupui/overlay/dialogoverlay.h>
#include <popupui/overlay/animations/fadepopupanimation.h>

using namespace PopupUI;
OverlayTest::OverlayTest(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    DialogOverlay::registerHostWindow(this);
    //DialogOverlay::enableMaskAnimation(false);
}

void OverlayTest::on_btn_show_clicked() {
    auto dlg = new OverlayDlg1;
    DialogOverlay::showDialog(dlg, new FadePopupAnimation);
}
