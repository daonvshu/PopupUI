#pragma once

#include <qwidget.h>

class ShadowWidget : public QWidget {
public:
    explicit ShadowWidget(QWidget* parent = nullptr);

    void showArrow(bool show);

protected:
    void setShadowRadius(int radius);

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

private:
    QImage createShadow(qreal blurRadius, const QColor& color, const QSizeF& contentSize, qreal cornerRadius, const QPointF& offset = QPointF(0, 0)) const;
    QPainterPath createArrowPath() const;
};