#include "dialogmask.h"

#include <qevent.h>
#include <qdebug.h>

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
        dlg->setAttribute(Qt::WA_DeleteOnClose, props.deleteOnClose);
        resizeTarget();
        moveTarget();
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
        if (props.draggableWidget) {
            auto me = static_cast<QMouseEvent*>(event);
            if (me->button() == Qt::LeftButton) {
                auto pos = me->pos();
                if (props.draggableWidget->geometry().contains(pos)) {
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
        resizeTarget();
        moveTarget();
    }
    QWidget::resizeEvent(event);
}

void DialogMask::resizeTarget() const {
    if (props.widthAspectRatio > 0 || props.heightAspectRatio > 0) {
        QSize targetSize = targetDlg->size();
        auto parentSize = size();
        if (props.widthAspectRatio > 0) {
            targetSize.setWidth(parentSize.width() * props.widthAspectRatio);
        }
        if (props.heightAspectRatio > 0) {
            targetSize.setHeight(parentSize.height() * props.heightAspectRatio);
        }
        targetDlg->resize(targetSize);
    }
}

void DialogMask::moveTarget() const {
    QPoint alignPos;
    bool alignPosValid = false;
    if (props.alignToTarget != nullptr) {
        alignPos = props.alignToTarget->mapToGlobal(props.alignToPos) - this->mapToGlobal(QPoint(0, 0));
        alignPosValid = true;
    }

    int targetX = 0;
    int targetY = 0;
    auto parentRect = rect();
    auto dlgSize = targetDlg->size();

    if (!alignPosValid) {
        targetX = parentRect.width() / 2 - dlgSize.width() / 2;
        targetY = parentRect.height() / 2 - dlgSize.height() / 2;
    } else {
        switch (props.edgeAlign) {
            case Qt::LeftEdge:
                targetX = alignPos.x();
                break;
            case Qt::RightEdge:
                targetX = alignPos.x() - dlgSize.width();
                break;
            case Qt::TopEdge:
                targetY = alignPos.y();
                break;
            case Qt::BottomEdge:
                targetY = alignPos.y() - dlgSize.height();
                break;
        }
        if (props.edgeAlign == Qt::TopEdge || props.edgeAlign == Qt::BottomEdge) {
            //horizontal
            if (props.posAlign & Qt::AlignLeft) {
                targetX = alignPos.x();
            } else if (props.posAlign & Qt::AlignRight) {
                targetX = alignPos.x() - dlgSize.width();
            } else {
                targetX = alignPos.x() - dlgSize.width() / 2;
            }
        } else {
            //vertical
            if (props.posAlign & Qt::AlignTop) {
                targetY = alignPos.y();
            } else if (props.posAlign & Qt::AlignBottom) {
                targetY = alignPos.y() - dlgSize.height();
            } else {
                targetY = alignPos.y() - dlgSize.height() / 2;
            }
        }
        if (targetX < 0) {
            targetX = 0;
        } else if (targetX + dlgSize.width() > parentRect.width()) {
            targetX = parentRect.width() - dlgSize.width();
        }
        if (targetY < 0) {
            targetY = 0;
        } else if (targetY + dlgSize.height() > parentRect.height()) {
            targetY = parentRect.height() - dlgSize.height();
        }
    }
    targetDlg->move(QPoint(targetX, targetY) + props.alignOffset);
}

POPUPUI_END_NAMESPACE
