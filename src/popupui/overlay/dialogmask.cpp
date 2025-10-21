#include "dialogmask.h"

#include <qevent.h>

POPUPUI_BEGIN_NAMESPACE

DialogMask::DialogMask(QWidget* parent, QWidget* dlg, const PopupProperty& props)
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

void DialogMask::unbindEvent() {
    if (targetDlg) {
        targetDlg->removeEventFilter(this);
    }
}

void DialogMask::mousePressEvent(QMouseEvent* event) {
    if (props.closable && targetDlg) {
        props.closable = false;
        unbindEvent();
        emit requestClose();
    }
}

bool DialogMask::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::Close) {
        if (watched == targetDlg) {
            event->ignore();
            unbindEvent();
            emit requestClose();
            return true;
        }
    } else if (event->type() == QEvent::Resize) {
        if (watched == parentWidget()) {
            setGeometry(parentWidget()->rect());
        }
    } else if (event->type() == QEvent::MouseButtonPress && watched == targetDlg) {
        if (props.draggableArea.isValid()) {
            auto me = static_cast<QMouseEvent*>(event);
            if (me->button() == Qt::LeftButton) {
                auto pos = me->pos();
                if (props.draggableArea.contains(pos)) {
                    dragging = true;
                    dragStartPosition = me->globalPos();
                    dragStartDialogPosition = targetDlg->pos();
                    return true;
                }
            }
        }
    } else if (event->type() == QEvent::MouseMove && watched == targetDlg) {
        if (dragging) {
            auto me = static_cast<QMouseEvent*>(event);
            auto offset = me->globalPos() - dragStartPosition;
            auto newPos = dragStartDialogPosition + offset;
            if (props.boundedDrag) {
                auto parentWin = targetDlg->parentWidget();
                if (parentWin) {
                    auto parentRect = parentWin->rect();

                    auto dlgSize = targetDlg->size();
                    int minX = parentRect.left();
                    int minY = parentRect.top();
                    int maxX = parentRect.right() - dlgSize.width();
                    int maxY = parentRect.bottom() - dlgSize.height();

                    if (newPos.x() < minX) newPos.setX(minX);
                    if (newPos.y() < minY) newPos.setY(minY);
                    if (newPos.x() > maxX) newPos.setX(maxX);
                    if (newPos.y() > maxY) newPos.setY(maxY);
                }
            }
            targetDlg->move(newPos);
            return true;
        }
    } else if (event->type() == QEvent::MouseButtonRelease && watched == targetDlg) {
        if (dragging) {
            dragging = false;
            return true;
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
