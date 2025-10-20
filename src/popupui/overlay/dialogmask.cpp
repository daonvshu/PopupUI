#include "dialogmask.h"

#include <qevent.h>

POPUPUI_BEGIN_NAMESPACE

DialogMask::DialogMask(QWidget* parent, QWidget* dlg, const Property& props)
    : QWidget(parent)
    , targetDlg(dlg)
    , props(props)
{
    setGeometry(parent->rect());
    parent->installEventFilter(this);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, props.color);
    setPalette(pal);
    setAutoFillBackground(true);

    setAttribute(Qt::WA_TransparentForMouseEvents, !props.interceptMouse);

    if (dlg) {
        dlg->installEventFilter(this);
        if (props.deleteOnClose) {
            dlg->setAttribute(Qt::WA_DeleteOnClose);
        }
        auto parentRect = parent->rect();
        auto dlgSize = dlg->size();
        QPoint centerPos((parentRect.width() - dlgSize.width()) / 2,
                         (parentRect.height() - dlgSize.height()) / 2);
        dlg->move(centerPos);
    }
}

void DialogMask::mousePressEvent(QMouseEvent* event) {
    if (props.closable && targetDlg) {
        props.closable = false;
        targetDlg->removeEventFilter(this);
        emit requestClose();
    }
}

bool DialogMask::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::Close) {
        if (watched == targetDlg) {
            event->ignore();
            targetDlg->removeEventFilter(this);
            emit requestClose();
            return true;
        }
    } else if (event->type() == QEvent::Resize) {
        if (watched == parentWidget()) {
            setGeometry(parentWidget()->rect());
        }
    }
    return QWidget::eventFilter(watched, event);
}

void DialogMask::resizeEvent(QResizeEvent* event) {
    if (targetDlg) {
        if (props.keepCenter) {
            auto parentRect = rect();
            auto dlgSize = targetDlg->size();
            QPoint centerPos((parentRect.width() - dlgSize.width()) / 2,
                             (parentRect.height() - dlgSize.height()) / 2);
            targetDlg->move(centerPos);
        }
    }
    QWidget::resizeEvent(event);
}

POPUPUI_END_NAMESPACE
