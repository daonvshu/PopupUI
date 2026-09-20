#pragma once

#include <popupui/comm/global.h>
#include <qwidget.h>

POPUPUI_BEGIN_NAMESPACE

class POPUPUI_EXPORT ShadowWidget : public QWidget {
public:
    explicit ShadowWidget(QWidget* parent = nullptr);

    void showArrow(bool show = true);
    void resizeContent(int width, int height);
    void setContentFixedSize(int width, int height);
    void setContentFixedWidth(int width);
    void setContentFixedHeight(int height);

protected:
    void setShadowRadius(int radius);
    void setAutoExpandShadowSize(bool enabled);
    void setAutoAdjustShadowPosition(bool enabled);

    bool event(QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

protected:
    int shadowRadius = 12;
    QColor shadowColor = 0xA9ACB6;
    int shadowCornerRadius = 8;
    QImage shadowImage;

    Qt::Alignment arrowPosAlign = Qt::AlignHCenter;
    Qt::Edge arrowEdgeAlign = Qt::TopEdge;
    Qt::Edges shadowEdges = Qt::Edge::LeftEdge | Qt::Edge::RightEdge | Qt::Edge::TopEdge | Qt::Edge::BottomEdge;
    QColor arrowFilledColor = Qt::white;
    bool isShowArrow = false;

protected:
    QImage createShadow(qreal blurRadius, const QColor& color, const QSizeF& contentSize, qreal cornerRadius, const QPointF& offset = QPointF(0, 0)) const;
    QPainterPath createArrowPath() const;

private:
    using QWidget::setFixedHeight;
    using QWidget::setFixedSize;
    using QWidget::setFixedWidth;

    QMargins shadowMargins() const;
    QSize outerSizeForContent(const QSize& contentSize) const;
    QSize contentSizeForOuter(const QSize& outerSize) const;
    void adjustSizeForShadowMargins(const QMargins& oldMargins, const QMargins& newMargins);
    void ensureShadowSizeExpanded();

private:
    bool autoExpandShadowSize = true;
    bool autoAdjustShadowPosition = true;
    bool shadowSizeExpanded = false;
    QMargins expandedShadowMargins;
};

POPUPUI_END_NAMESPACE
