#include "overlaydlg2.h"

#include <qstyleoption.h>
#include <qpainter.h>
#include <qdebug.h>

OverlayDlg2::OverlayDlg2(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
}

OverlayDlg2::~OverlayDlg2() {
    qDebug() << "overlay dlg2 release...";
}

void OverlayDlg2::on_btn_close_clicked() {
    close();
}

void OverlayDlg2::paintEvent(QPaintEvent* event) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
