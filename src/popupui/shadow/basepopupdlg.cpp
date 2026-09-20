#include "basepopupdlg.h"

#include <qevent.h>
#include <qguiapplication.h>
#include <qscreen.h>

POPUPUI_BEGIN_NAMESPACE

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
        QScreen* screen = parentWidget() ? parentWidget()->screen() : QGuiApplication::primaryScreen();
        if (screen) {
            const QRect screenRect = screen->availableGeometry();
            move(screenRect.center() - rect().center());
        }
    }
    if (!isAnimating) {
        // 延迟一帧执行动画，确保窗口几何信息已就绪
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
    if (!screen && parentWidget()) {
        screen = parentWidget()->screen();
    }
    if (!screen) {
        screen = QGuiApplication::primaryScreen();
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
        // 真正关闭
        emit popClosed();
        QWidget::closeEvent(event);
        reallyClose = false;
        return;
    }

    // 拦截关闭，先播放动画
    event->ignore();
    playHideAnimation();
}

void BasePopupDlg::setupAnimations() {
    // === 显示动画组 ===
    showAnimGroup = new QParallelAnimationGroup(this);

    // 透明度动画
    fadeInAnim = new QPropertyAnimation(this, "windowOpacity");
    fadeInAnim->setDuration(animDuration);
    fadeInAnim->setStartValue(0.0);
    fadeInAnim->setEndValue(1.0);
    fadeInAnim->setEasingCurve(QEasingCurve::OutCubic);

    // 位置动画
    slideUpAnim = new QPropertyAnimation(this, "geometry");
    slideUpAnim->setDuration(animDuration);
    slideUpAnim->setEasingCurve(QEasingCurve::OutCubic);

    showAnimGroup->addAnimation(fadeInAnim);
    showAnimGroup->addAnimation(slideUpAnim);

    connect(showAnimGroup, &QParallelAnimationGroup::finished, this, &BasePopupDlg::onShowAnimationFinished);

    // === 隐藏动画组 ===
    hideAnimGroup = new QParallelAnimationGroup(this);

    // 透明度动画
    fadeOutAnim = new QPropertyAnimation(this, "windowOpacity");
    fadeOutAnim->setDuration(animDuration * 0.8); // 隐藏稍快
    fadeOutAnim->setStartValue(1.0);
    fadeOutAnim->setEndValue(0.0);
    fadeOutAnim->setEasingCurve(QEasingCurve::InCubic);

    // 位置动画
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

    // 保存目标位置
    QRect targetGeometry = geometry();

    // 设置起始位置（下方）
    QRect startGeometry = targetGeometry;
    startGeometry.moveTop(targetGeometry.top() + slideDistance);

    // 先设置到起始状态
    setGeometry(startGeometry);
    setWindowOpacity(0.0);

    // 配置动画
    slideUpAnim->setStartValue(startGeometry);
    slideUpAnim->setEndValue(targetGeometry);

    // 启动动画
    showAnimGroup->start();
}

void BasePopupDlg::playHideAnimation() {
    if (isAnimating) return;

    isAnimating = true;

    // 当前位置
    QRect currentGeometry = geometry();

    // 目标位置（下方）
    QRect endGeometry = currentGeometry;
    endGeometry.moveTop(currentGeometry.top() + slideDistance * 0.6);

    // 配置动画
    slideDownAnim->setStartValue(currentGeometry);
    slideDownAnim->setEndValue(endGeometry);

    // 启动动画
    hideAnimGroup->start();
}

void BasePopupDlg::onShowAnimationFinished() {
    isAnimating = false;
    setWindowOpacity(1.0); // 确保完全不透明
}

void BasePopupDlg::onHideAnimationFinished() {
    isAnimating = false;
    reallyClose = true;
    close(); // 真正隐藏
}

POPUPUI_END_NAMESPACE
