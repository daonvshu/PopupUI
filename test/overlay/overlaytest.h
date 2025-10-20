#pragma once

#include <qwidget.h>

#include "ui_overlaytest.h"

class OverlayTest : public QWidget {
    Q_OBJECT

public:
    explicit OverlayTest(QWidget* parent = nullptr);

private slots:
    void on_btn_show_clicked();

private:
    Ui::OverlayTest ui;
};
