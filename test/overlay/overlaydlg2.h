#pragma once

#include <qwidget.h>

#include "ui_overlaydlg2.h"

class OverlayDlg2 : public QWidget {
    Q_OBJECT

public:
    explicit OverlayDlg2(QWidget* parent = nullptr);
    ~OverlayDlg2();

    QWidget* getDraggableTitle() const;

private slots:
    void on_btn_close_clicked();
    void on_btn_close_top_clicked();
    void on_btn_close_all_clicked();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Ui::OverlayDlg2 ui;
};