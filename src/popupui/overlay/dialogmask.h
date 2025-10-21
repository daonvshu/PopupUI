#pragma once

#include <qwidget.h>
#include <popupui/comm/global.h>

#include "popupproperty.h"

POPUPUI_BEGIN_NAMESPACE

class DialogMask : public QWidget {
    Q_OBJECT

public:
    explicit DialogMask(QWidget* parent, QWidget* dlg, const PopupProperty& props);
    void unbindEvent();

signals:
    void requestClose();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    QWidget* targetDlg;
    PopupProperty props;
    bool dragging = false;
    QPoint dragStartPosition;
    QPoint dragStartDialogPosition;
};

POPUPUI_END_NAMESPACE