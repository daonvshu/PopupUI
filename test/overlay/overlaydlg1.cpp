#include "overlaydlg1.h"
#include "overlaydlg2.h"

#include <popupui/overlay/dialogoverlay.h>
#include <qstyleoption.h>
#include <qpainter.h>
#include <qdebug.h>

#include <popupui/overlay/animations/fadepopupanimation.h>
#include <popupui/overlay/animations/scalepopupanimation.h>
#include <popupui/overlay/animations/slidepopupanimation.h>
#include <popupui/overlay/animations/parallelpopupanimation.h>

using namespace PopupUI;

OverlayDlg1::OverlayDlg1(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
}

OverlayDlg1::~OverlayDlg1() {
    qDebug() << "overlay dlg1 release...";
}

void OverlayDlg1::on_btn_show2_clicked() {
    auto dlg = new OverlayDlg2;
    DialogOverlay::showDialog(dlg,
        //new FadePopupAnimation,
        //new ScalePopupAnimation,
        //new SlidePopupAnimation(SlideDirection::FromRight),
        new ParallelPopupAnimation({ new FadePopupAnimation, new ScalePopupAnimation }),
        PopupProperty()
            .closeOnClickOutside()
            .setMaskColor(QColor(255, 170, 255, 50))
            .setDraggableArea(dlg->getDraggableArea())
    );
}

void OverlayDlg1::on_btn_show2_exec_clicked() {
    OverlayDlg2 dlg;
    DialogOverlay::showDialogExec(dlg,
        new FadePopupAnimation,
        PopupProperty()
            .closeOnClickOutside()
            .setMaskColor(QColor(255, 170, 255, 50))
            .setDraggableArea(dlg.getDraggableArea())
    );
    qDebug() << "overlay dlg2 exec finished!";
}

void OverlayDlg1::on_btn_close_clicked() {
    close();
}

void OverlayDlg1::paintEvent(QPaintEvent* event) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
