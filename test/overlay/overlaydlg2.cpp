#include "overlaydlg2.h"

#include <qstyleoption.h>
#include <qpainter.h>
#include <qdebug.h>

#include <popupui/overlay/dialogoverlay.h>

using namespace PopupUI;
OverlayDlg2::OverlayDlg2(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
}

OverlayDlg2::~OverlayDlg2() {
    qDebug() << "overlay dlg2 release...";
}

QRectF OverlayDlg2::getDraggableArea() const {
    return ui.drag_area->geometry();
}

void OverlayDlg2::on_btn_close_clicked() {
    close();
}

void OverlayDlg2::on_btn_close_top_clicked() {
    DialogOverlay::closeTopDialog();
}

void OverlayDlg2::on_btn_close_all_clicked() {
    DialogOverlay::clear();
}

void OverlayDlg2::paintEvent(QPaintEvent* event) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
