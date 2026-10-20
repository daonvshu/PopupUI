#include "basepopupdlg.h"

#include <qevent.h>
#include <qguiapplication.h>
#include <qscreen.h>
#include <qwindow.h>

POPUPUI_BEGIN_NAMESPACE

namespace {

QScreen* screenForWidget(const QWidget* widget) {
    if (widget) {
        const auto* window = widget->window()->windowHandle();
        if (window && window->screen()) {
            return window->screen();
        }
        if (auto* screen = QGuiApplication::screenAt(widget->mapToGlobal(widget->rect().center()))) {
            return screen;
        }
    }
    return QGuiApplication::primaryScreen();
}

} // namespace

BasePopupDlg::BasePopupDlg(QWidget* parent)
    : ShadowWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(windowFlags() | Qt::Popup | Qt::WindowStaysOnTopHint | Qt::NoDropShadowWindowHint | Qt::FramelessWindowHint);

    setupAnimations();
}

void BasePopupDlg::moveTo(const QPoint& pos, Qt::Edge edgeAlign, Qt::Alignment posAlign) {
    initPos = pos;
    hasPlacementRequest = true;
    hasAnchorRect = false;
    arrowEdgeAlign = edgeAlign;
    arrowPosAlign = posAlign;
}

void BasePopupDlg::moveToTarget(const QWidget* target, Qt::Edge edgeAlign, Qt::Alignment posAlign, const QPoint& offset) {
    if (!target) {
        return;
    }
    moveToTargetRect(QRect(target->mapToGlobal(QPoint(0, 0)), target->size()), edgeAlign, posAlign, offset);
}

void BasePopupDlg::moveToTargetBottom(const QWidget* target, Qt::Alignment posAlign, int yOffset) {
    moveToTarget(target, Qt::TopEdge, posAlign, QPoint(0, yOffset));
}

void BasePopupDlg::moveToTargetTop(const QWidget* target, Qt::Alignment posAlign, int yOffset) {
    moveToTarget(target, Qt::BottomEdge, posAlign, QPoint(0, yOffset));
}

void BasePopupDlg::moveToTargetLeft(const QWidget* target, Qt::Alignment posAlign, int xOffset) {
    moveToTarget(target, Qt::RightEdge, posAlign, QPoint(xOffset, 0));
}

void BasePopupDlg::moveToTargetRight(const QWidget* target, Qt::Alignment posAlign, int xOffset) {
    moveToTarget(target, Qt::LeftEdge, posAlign, QPoint(xOffset, 0));
}

void BasePopupDlg::moveToTargetBottom(const QRect& targetRectGlobal, Qt::Alignment posAlign, int yOffset) {
    moveToTargetRect(targetRectGlobal, Qt::TopEdge, posAlign, QPoint(0, yOffset));
}

void BasePopupDlg::moveToTargetTop(const QRect& targetRectGlobal, Qt::Alignment posAlign, int yOffset) {
    moveToTargetRect(targetRectGlobal, Qt::BottomEdge, posAlign, QPoint(0, yOffset));
}

void BasePopupDlg::moveToTargetLeft(const QRect& targetRectGlobal, Qt::Alignment posAlign, int xOffset) {
    moveToTargetRect(targetRectGlobal, Qt::RightEdge, posAlign, QPoint(xOffset, 0));
}

void BasePopupDlg::moveToTargetRight(const QRect& targetRectGlobal, Qt::Alignment posAlign, int xOffset) {
    moveToTargetRect(targetRectGlobal, Qt::LeftEdge, posAlign, QPoint(xOffset, 0));
}

void BasePopupDlg::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
}

void BasePopupDlg::showEvent(QShowEvent* event) {
    if (hasPlacementRequest) {
        auto edgeAlign = arrowEdgeAlign;
        auto posAlign = arrowPosAlign;
        const QRect screenRect = screenAvailableRectForPlacement();
        auto popupRect = calcPopupRect(edgeAlign, posAlign);

        if (!screenRect.isEmpty()) {
            if (edgeAlign == Qt::TopEdge || edgeAlign == Qt::BottomEdge) {
                const bool overflowDown = popupRect.bottom() > screenRect.bottom();
                const bool overflowUp = popupRect.top() < screenRect.top();
                if ((edgeAlign == Qt::TopEdge && overflowDown) || (edgeAlign == Qt::BottomEdge && overflowUp)) {
                    const auto flippedEdge = edgeAlign == Qt::TopEdge ? Qt::BottomEdge : Qt::TopEdge;
                    const auto flippedRect = calcPopupRect(flippedEdge, posAlign);
                    if (screenRect.contains(flippedRect)) {
                        edgeAlign = flippedEdge;
                        popupRect = flippedRect;
                    } else {
                        const int belowSpace = qMax(0, screenRect.bottom() - calcAnchorPoint(Qt::TopEdge, posAlign).y());
                        const int aboveSpace = qMax(0, calcAnchorPoint(Qt::BottomEdge, posAlign).y() - screenRect.top());
                        edgeAlign = aboveSpace > belowSpace ? Qt::BottomEdge : Qt::TopEdge;
                        popupRect = calcPopupRect(edgeAlign, posAlign);
                    }
                }

                if (popupRect.right() > screenRect.right()) {
                    const auto alignedRect = calcPopupRect(edgeAlign, Qt::AlignRight);
                    if (screenRect.contains(alignedRect)) {
                        posAlign = Qt::AlignRight;
                        popupRect = alignedRect;
                    }
                } else if (popupRect.left() < screenRect.left()) {
                    const auto alignedRect = calcPopupRect(edgeAlign, Qt::AlignLeft);
                    if (screenRect.contains(alignedRect)) {
                        posAlign = Qt::AlignLeft;
                        popupRect = alignedRect;
                    }
                }
            } else {
                const bool overflowRight = popupRect.right() > screenRect.right();
                const bool overflowLeft = popupRect.left() < screenRect.left();
                if ((edgeAlign == Qt::LeftEdge && overflowRight) || (edgeAlign == Qt::RightEdge && overflowLeft)) {
                    const auto flippedEdge = edgeAlign == Qt::LeftEdge ? Qt::RightEdge : Qt::LeftEdge;
                    const auto flippedRect = calcPopupRect(flippedEdge, posAlign);
                    if (screenRect.contains(flippedRect)) {
                        edgeAlign = flippedEdge;
                        popupRect = flippedRect;
                    } else {
                        const int rightSpace = qMax(0, screenRect.right() - calcAnchorPoint(Qt::LeftEdge, posAlign).x());
                        const int leftSpace = qMax(0, calcAnchorPoint(Qt::RightEdge, posAlign).x() - screenRect.left());
                        edgeAlign = leftSpace > rightSpace ? Qt::RightEdge : Qt::LeftEdge;
                        popupRect = calcPopupRect(edgeAlign, posAlign);
                    }
                }

                if (popupRect.bottom() > screenRect.bottom()) {
                    const auto alignedRect = calcPopupRect(edgeAlign, Qt::AlignBottom);
                    if (screenRect.contains(alignedRect)) {
                        posAlign = Qt::AlignBottom;
                        popupRect = alignedRect;
                    }
                } else if (popupRect.top() < screenRect.top()) {
                    const auto alignedRect = calcPopupRect(edgeAlign, Qt::AlignTop);
                    if (screenRect.contains(alignedRect)) {
                        posAlign = Qt::AlignTop;
                        popupRect = alignedRect;
                    }
                }
            }

            popupRect.moveLeft(popupRect.width() <= screenRect.width()
                ? qBound(screenRect.left(), popupRect.left(), screenRect.right() - popupRect.width() + 1)
                : screenRect.left());
            popupRect.moveTop(popupRect.height() <= screenRect.height()
                ? qBound(screenRect.top(), popupRect.top(), screenRect.bottom() - popupRect.height() + 1)
                : screenRect.top());
        }

        arrowEdgeAlign = edgeAlign;
        arrowPosAlign = posAlign;
        move(popupRect.topLeft());
    } else {
        QScreen* screen = screenForWidget(parentWidget());
        if (screen) {
            const QRect screenRect = screen->availableGeometry();
            move(screenRect.center() - rect().center());
        }
    }
    if (!isAnimating) {
        // Defer the animation until the window geometry is ready
        QMetaObject::invokeMethod(this, &BasePopupDlg::playShowAnimation, Qt::QueuedConnection);
    }
    if (!(windowFlags() & Qt::WindowDoesNotAcceptFocus)) {
        activateWindow();
        setFocus();
    }
}

QPoint BasePopupDlg::calcAnchorPoint(Qt::Edge edgeAlign, Qt::Alignment posAlign) const {
    if (!hasAnchorRect) {
        return initPos;
    }

    int x = anchorRect.left();
    int y = anchorRect.top();
    if (edgeAlign == Qt::TopEdge || edgeAlign == Qt::BottomEdge) {
        if (posAlign & Qt::AlignHCenter) {
            x = anchorRect.center().x();
        } else if (posAlign & Qt::AlignRight) {
            x = anchorRect.right() + 1;
        }
        y = edgeAlign == Qt::TopEdge
            ? anchorRect.bottom() + 1 + anchorOffset.y()
            : anchorRect.top() - anchorOffset.y();
    } else {
        if (posAlign & Qt::AlignVCenter) {
            y = anchorRect.center().y();
        } else if (posAlign & Qt::AlignBottom) {
            y = anchorRect.bottom() + 1;
        }
        x = edgeAlign == Qt::LeftEdge
            ? anchorRect.right() + 1 + anchorOffset.x()
            : anchorRect.left() - anchorOffset.x();
    }
    return QPoint(x, y);
}

QRect BasePopupDlg::calcPopupRect(Qt::Edge edgeAlign, Qt::Alignment posAlign) const {
    const QPoint anchorPos = calcAnchorPoint(edgeAlign, posAlign);
    int targetX = anchorPos.x();
    int targetY = anchorPos.y();

    if (edgeAlign == Qt::LeftEdge) {
        targetX -= shadowRadius;
    } else if (edgeAlign == Qt::RightEdge) {
        targetX -= width() - shadowRadius;
    } else if (edgeAlign == Qt::TopEdge) {
        targetY -= shadowRadius;
    } else {
        targetY -= height() - shadowRadius;
    }

    if (edgeAlign == Qt::TopEdge || edgeAlign == Qt::BottomEdge) {
        if (posAlign & Qt::AlignLeft) {
            targetX = anchorPos.x() - shadowRadius;
        } else if (posAlign & Qt::AlignHCenter) {
            targetX = anchorPos.x() - width() / 2;
        } else if (posAlign & Qt::AlignRight) {
            targetX = anchorPos.x() - width() + shadowRadius;
        }
    } else {
        if (posAlign & Qt::AlignTop) {
            targetY = anchorPos.y() - shadowRadius;
        } else if (posAlign & Qt::AlignVCenter) {
            targetY = anchorPos.y() - height() / 2;
        } else if (posAlign & Qt::AlignBottom) {
            targetY = anchorPos.y() - height() + shadowRadius;
        }
    }

    return QRect(QPoint(targetX, targetY), size());
}

QRect BasePopupDlg::screenAvailableRectForPlacement() const {
    QScreen* screen = QGuiApplication::screenAt(calcAnchorPoint(arrowEdgeAlign, arrowPosAlign));
    if (!screen) {
        screen = screenForWidget(parentWidget());
    }
    return screen ? screen->availableGeometry() : QRect();
}

void BasePopupDlg::moveToTargetRect(const QRect& targetRectGlobal, Qt::Edge edgeAlign, Qt::Alignment posAlign, const QPoint& offset) {
    if (!targetRectGlobal.isValid()) {
        return;
    }

    hasPlacementRequest = true;
    hasAnchorRect = true;
    anchorRect = targetRectGlobal;
    anchorOffset = offset;
    arrowEdgeAlign = edgeAlign;
    arrowPosAlign = posAlign;
    initPos = calcAnchorPoint(arrowEdgeAlign, arrowPosAlign);
}

void BasePopupDlg::closeEvent(QCloseEvent* event) {
    if (reallyClose) {
        // Complete the close operation
        emit popClosed();
        QWidget::closeEvent(event);
        reallyClose = false;
        return;
    }

    // Intercept closing to play the hide animation first
    event->ignore();
    playHideAnimation();
}

void BasePopupDlg::setupAnimations() {
    // === Show animation group ===
    showAnimGroup = new QParallelAnimationGroup(this);

    // Opacity animation
    fadeInAnim = new QPropertyAnimation(this, "windowOpacity");
    fadeInAnim->setDuration(animDuration);
    fadeInAnim->setStartValue(0.0);
    fadeInAnim->setEndValue(1.0);
    fadeInAnim->setEasingCurve(QEasingCurve::OutCubic);

    // Position animation
    slideUpAnim = new QPropertyAnimation(this, "geometry");
    slideUpAnim->setDuration(animDuration);
    slideUpAnim->setEasingCurve(QEasingCurve::OutCubic);

    showAnimGroup->addAnimation(fadeInAnim);
    showAnimGroup->addAnimation(slideUpAnim);

    connect(showAnimGroup, &QParallelAnimationGroup::finished, this, &BasePopupDlg::onShowAnimationFinished);

    // === Hide animation group ===
    hideAnimGroup = new QParallelAnimationGroup(this);

    // Opacity animation
    fadeOutAnim = new QPropertyAnimation(this, "windowOpacity");
    fadeOutAnim->setDuration(animDuration * 0.8); // Hide slightly faster
    fadeOutAnim->setStartValue(1.0);
    fadeOutAnim->setEndValue(0.0);
    fadeOutAnim->setEasingCurve(QEasingCurve::InCubic);

    // Position animation
    slideDownAnim = new QPropertyAnimation(this, "geometry");
    slideDownAnim->setDuration(animDuration * 0.8);
    slideDownAnim->setEasingCurve(QEasingCurve::InCubic);

    hideAnimGroup->addAnimation(fadeOutAnim);
    hideAnimGroup->addAnimation(slideDownAnim);

    connect(hideAnimGroup, &QParallelAnimationGroup::finished, this, &BasePopupDlg::onHideAnimationFinished);
}

void BasePopupDlg::playShowAnimation() {
    if (isAnimating) return;

    isAnimating = true;

    // Save the target geometry
    QRect targetGeometry = geometry();

    // Place the starting geometry below the target
    QRect startGeometry = targetGeometry;
    startGeometry.moveTop(targetGeometry.top() + slideDistance);

    // Apply the initial state
    setGeometry(startGeometry);
    setWindowOpacity(0.0);

    // Configure the animation
    slideUpAnim->setStartValue(startGeometry);
    slideUpAnim->setEndValue(targetGeometry);

    // Start the animation
    showAnimGroup->start();
}

void BasePopupDlg::playHideAnimation() {
    if (isAnimating) return;

    isAnimating = true;

    // Current geometry
    QRect currentGeometry = geometry();

    // Target geometry below the current position
    QRect endGeometry = currentGeometry;
    endGeometry.moveTop(currentGeometry.top() + slideDistance * 0.6);

    // Configure the animation
    slideDownAnim->setStartValue(currentGeometry);
    slideDownAnim->setEndValue(endGeometry);

    // Start the animation
    hideAnimGroup->start();
}

void BasePopupDlg::onShowAnimationFinished() {
    isAnimating = false;
    setWindowOpacity(1.0); // Ensure full opacity
}

void BasePopupDlg::onHideAnimationFinished() {
    isAnimating = false;
    reallyClose = true;
    close(); // Complete the close operation
}

POPUPUI_END_NAMESPACE
