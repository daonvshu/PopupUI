#pragma once

#include <popupui/comm/global.h>
#include <qwidget.h>
#include <qparallelanimationgroup.h>
#include <qpropertyanimation.h>

#include "shadowwidget.h"

POPUPUI_BEGIN_NAMESPACE

class POPUPUI_EXPORT BasePopupDlg : public ShadowWidget {
    Q_OBJECT

public:
    explicit BasePopupDlg(QWidget* parent = nullptr);

    void moveTo(const QPoint& pos, Qt::Edge edgeAlign = Qt::TopEdge, Qt::Alignment posAlign = Qt::AlignHCenter);
    void moveToTargetBottom(const QWidget* target, Qt::Alignment posAlign = Qt::AlignLeft, int yOffset = 4);
    void moveToTargetTop(const QWidget* target, Qt::Alignment posAlign = Qt::AlignLeft, int yOffset = 4);
    void moveToTargetLeft(const QWidget* target, Qt::Alignment posAlign = Qt::AlignTop, int xOffset = 4);
    void moveToTargetRight(const QWidget* target, Qt::Alignment posAlign = Qt::AlignTop, int xOffset = 4);

    void moveToTargetBottom(const QRect& targetRectGlobal, Qt::Alignment posAlign = Qt::AlignLeft, int yOffset = 4);
    void moveToTargetTop(const QRect& targetRectGlobal, Qt::Alignment posAlign = Qt::AlignLeft, int yOffset = 4);
    void moveToTargetLeft(const QRect& targetRectGlobal, Qt::Alignment posAlign = Qt::AlignTop, int xOffset = 4);
    void moveToTargetRight(const QRect& targetRectGlobal, Qt::Alignment posAlign = Qt::AlignTop, int xOffset = 4);

signals:
    void popClosed();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

protected:
    QPoint initPos;
    QRect anchorRect;
    QPoint anchorOffset;
    bool hasPlacementRequest = false;
    bool hasAnchorRect = false;

    QParallelAnimationGroup *showAnimGroup;
    QParallelAnimationGroup *hideAnimGroup;

    QPropertyAnimation *fadeInAnim;
    QPropertyAnimation *slideUpAnim;
    QPropertyAnimation *fadeOutAnim;
    QPropertyAnimation *slideDownAnim;

    bool isAnimating = false;       // Animation in progress
    bool reallyClose = false;       // Allow closing after the hide animation
    int animDuration = 180;         // Animation duration
    int slideDistance = 30;         // Slide distance

protected:
    void setupAnimations();
    void playShowAnimation();
    void playHideAnimation();

    void onShowAnimationFinished();
    void onHideAnimationFinished();

private:
    void moveToTarget(const QWidget* target, Qt::Edge edgeAlign, Qt::Alignment posAlign, const QPoint& offset);
    void moveToTargetRect(const QRect& targetRectGlobal, Qt::Edge edgeAlign, Qt::Alignment posAlign, const QPoint& offset);
    QPoint calcAnchorPoint(Qt::Edge edgeAlign, Qt::Alignment posAlign) const;
    QRect calcPopupRect(Qt::Edge edgeAlign, Qt::Alignment posAlign) const;
    QRect screenAvailableRectForPlacement() const;
};

POPUPUI_END_NAMESPACE
