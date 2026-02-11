#pragma once

#include <qwidget.h>
#include <qparallelanimationgroup.h>
#include <qpropertyanimation.h>

#include "shadowwidget.h"

class BasePopupDlg : public ShadowWidget {
public:
    explicit BasePopupDlg(QWidget* parent = nullptr);

    void moveTo(const QPoint& pos, Qt::Edge edgeAlign = Qt::TopEdge, Qt::Alignment posAlign = Qt::AlignHCenter);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

protected:
    QPoint initPos;

    QParallelAnimationGroup *showAnimGroup;
    QParallelAnimationGroup *hideAnimGroup;

    QPropertyAnimation *fadeInAnim;
    QPropertyAnimation *slideUpAnim;
    QPropertyAnimation *fadeOutAnim;
    QPropertyAnimation *slideDownAnim;

    bool isAnimating = false;       // 动画进行中标志
    bool reallyClose = false;       // 真正关闭标志
    int animDuration = 180;         // 动画时长
    int slideDistance = 30;         // 滑动距离

protected:
    void setupAnimations();
    void playShowAnimation();
    void playHideAnimation();

    void onShowAnimationFinished();
    void onHideAnimationFinished();
};