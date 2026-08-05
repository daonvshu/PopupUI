#pragma once

#include <qpointer.h>
#include <qwidget.h>

#include "ui_overlaytest.h"

class OverlayTest : public QWidget {
    Q_OBJECT

public:
    explicit OverlayTest(QWidget* parent = nullptr);

private slots:
    void on_btn_show_clicked();
    void on_btn_create_dynamic_host_clicked();
    void on_btn_show_dynamic_host_clicked();
    void on_btn_destroy_dynamic_host_clicked();

private:
    Ui::OverlayTest ui;
    QPointer<QWidget> dynamicHost;
};
