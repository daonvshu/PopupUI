#include "basepopupdlg.h"

#include <qevent.h>

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
    arrowEdgeAlign = edgeAlign;
    arrowPosAlign = posAlign;
}

void BasePopupDlg::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
}

void BasePopupDlg::showEvent(QShowEvent* event) {
    if (!initPos.isNull()) {
        int targetX = initPos.x();
        int targetY = initPos.y();

        switch (arrowEdgeAlign) {
            case Qt::LeftEdge:
                targetX = initPos.x() - shadowRadius;
                break;
            case Qt::RightEdge:
                targetX = initPos.x() - width() + 2 * shadowRadius;
                break;
            case Qt::TopEdge:
                targetY = initPos.y() - shadowRadius;
                break;
            case Qt::BottomEdge:
                targetY = initPos.y() - height() + 2 * shadowRadius;
                break;
        }

        if (arrowEdgeAlign == Qt::TopEdge || arrowEdgeAlign == Qt::BottomEdge) {
            //处理水平X方向
            if (arrowPosAlign & Qt::AlignLeft) {
                targetX = initPos.x() - shadowRadius;
            } else if (arrowPosAlign & Qt::AlignHCenter) {
                targetX = initPos.x() - width() / 2;
            } else if (arrowPosAlign & Qt::AlignRight) {
                targetX = initPos.x() - width() + 2 * shadowRadius;
            }
        } else {
            //处理垂直Y方向
            if (arrowPosAlign & Qt::AlignTop) {
                targetY = initPos.y() - shadowRadius;
            } else if (arrowPosAlign & Qt::AlignVCenter) {
                targetY = initPos.y() - height() / 2;
            } else if (arrowPosAlign & Qt::AlignBottom) {
                targetY = initPos.y() - height() + 2 * shadowRadius;
            }
        }
        move(targetX, targetY);
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

void BasePopupDlg::closeEvent(QCloseEvent* event) {
    if (reallyClose) {
        // 真正关闭
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
