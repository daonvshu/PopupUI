#pragma once

#include <qwidget.h>

#include "ui_overlaydlg1.h"

class OverlayDlg1 : public QWidget {
    Q_OBJECT
public:
    explicit OverlayDlg1(QWidget* parent = nullptr);
    ~OverlayDlg1();

private slots:
    void on_btn_show2_clicked();
    void on_btn_close_clicked();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Ui::OverlayDlg1 ui;
};