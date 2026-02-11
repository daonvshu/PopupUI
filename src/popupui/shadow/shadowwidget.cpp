#include "shadowwidget.h"

#include <qevent.h>
#include <qmath.h>
#include <qpainter.h>
#include <qstyleoption.h>
#include <qpainterpath.h>
#include <qdebug.h>

// Qt internal function (qtbase/src/widgets/effects/qpixmapfilter.cpp)
extern void qt_blurImage(QPainter* p, QImage& blurImage, qreal radius, bool quality, bool alphaOnly, int transposed);

ShadowWidget::ShadowWidget(QWidget* parent)
    : QWidget(parent)
{
    setContentsMargins(shadowRadius, shadowRadius, shadowRadius, shadowRadius);
}

void ShadowWidget::showArrow(bool show) {
    isShowArrow = show;
}

void ShadowWidget::setShadowRadius(int radius) {
    shadowRadius = radius;
    shadowImage = QImage();
    setContentsMargins(
        shadowEdges & Qt::LeftEdge ? shadowRadius : 0,
        shadowEdges & Qt::TopEdge ? shadowRadius : 0,
        shadowEdges & Qt::RightEdge ? shadowRadius : 0,
        shadowEdges & Qt::BottomEdge ? shadowRadius : 0
    );
    repaint();
}

void ShadowWidget::paintEvent(QPaintEvent* event) {
    if (shadowImage.isNull() || shadowImage.size() != size()) {
        shadowImage = createShadow(shadowRadius, shadowColor, size() - QSize(2 * shadowRadius, 2 * shadowRadius), shadowCornerRadius);
    }
    QPainter p(this);
    p.drawImage(0, 0, shadowImage);

    //draw arrow
    p.save();
    p.setRenderHint(QPainter::Antialiasing);
    p.fillPath(createArrowPath(), arrowFilledColor);
    p.restore();

    QStyleOption opt;
    opt.init(this);
    opt.rect.adjust(
        shadowEdges & Qt::LeftEdge ? shadowRadius : 0,
        shadowEdges & Qt::TopEdge ? shadowRadius : 0,
        shadowEdges & Qt::RightEdge ? -shadowRadius : 0,
        shadowEdges & Qt::BottomEdge ? -shadowRadius : 0
    );
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

QImage ShadowWidget::createShadow(qreal blurRadius, const QColor& color, const QSizeF& contentSize, qreal cornerRadius, const QPointF& offset) const {
    if (contentSize.isEmpty() || blurRadius <= 0.0)
        return QImage();

    const qreal dpr = devicePixelRatioF();

    // ---------- 1. logical sizes ----------
    const QSizeF logicalImageSize = contentSize + QSizeF(blurRadius * 2, blurRadius * 2);
    const QSize imageSizePx = (logicalImageSize * dpr).toSize();

    // ---------- 2. mask image (device pixel) ----------
    QImage mask(imageSizePx, QImage::Format_ARGB32_Premultiplied);
    mask.fill(Qt::transparent);

    // ---------- 3. build path ----------
    const QRectF rect(QPointF(blurRadius, blurRadius) + offset, contentSize);
    QPainterPath path;
    path.addRoundedRect(rect, cornerRadius, cornerRadius);
    path.addPath(createArrowPath());

    // ---------- 4. draw mask ----------
    {
        QPainter p(&mask);
        p.setRenderHint(QPainter::Antialiasing);
        p.scale(dpr, dpr);
        p.setPen(Qt::NoPen);
        p.setBrush(Qt::white);   // alpha = 1.0
        p.drawPath(path);
    }

    // ---------- 5. blur ----------
    QImage blurred(mask.size(), QImage::Format_ARGB32_Premultiplied);
    blurred.fill(Qt::transparent);
    {
        QPainter p(&blurred);
        qt_blurImage(&p, mask, blurRadius * dpr, false, true, 0);
    }

    // ---------- 6. tint ----------
    {
        QPainter p(&blurred);
        p.setCompositionMode(QPainter::CompositionMode_SourceIn);
        p.fillRect(blurred.rect(), color);
    }

    return blurred;
}

QPainterPath ShadowWidget::createArrowPath() const {
    if (!isShowArrow) {
        return QPainterPath();
    }

    const qreal r  = 2.0;                    // 圆角半径
    const qreal aw = 7.0;                    // 箭头半高/半宽
    const qreal offset = shadowRadius + shadowCornerRadius + 12; // 避边距离（防止贴边）
    const qreal sr = shadowRadius;

    QPainterPath path;
    QPointF base; // 箭头基点（底边中点）

    // ---- Step 1: 根据箭头边方向计算基点 ----
    switch (arrowEdgeAlign) {
    case Qt::TopEdge:
        // 箭头在上边，垂直方向固定在 shadowRadius
        base.setY(sr);
        // 水平位置（左、中、右）
        if (arrowPosAlign & Qt::AlignLeft)
            base.setX(offset);
        else if (arrowPosAlign & Qt::AlignRight)
            base.setX(width() - offset);
        else
            base.setX(width() / 2.0);
        break;

    case Qt::BottomEdge:
        base.setY(height() - sr);
        if (arrowPosAlign & Qt::AlignLeft)
            base.setX(offset);
        else if (arrowPosAlign & Qt::AlignRight)
            base.setX(width() - offset);
        else
            base.setX(width() / 2.0);
        break;

    case Qt::LeftEdge:
        base.setX(sr);
        if (arrowPosAlign & Qt::AlignTop)
            base.setY(offset);
        else if (arrowPosAlign & Qt::AlignBottom)
            base.setY(height() - offset);
        else
            base.setY(height() / 2.0);
        break;

    case Qt::RightEdge:
        base.setX(width() - sr);
        if (arrowPosAlign & Qt::AlignTop)
            base.setY(offset);
        else if (arrowPosAlign & Qt::AlignBottom)
            base.setY(height() - offset);
        else
            base.setY(height() / 2.0);
        break;
    }

    // ---- Step 2: 根据边方向绘制箭头路径 ----
    switch (arrowEdgeAlign) {
    case Qt::TopEdge:
        path.moveTo(base.x() - aw, base.y());
        path.lineTo(base.x(), base.y() - aw + r);
        path.quadTo(base.x(), base.y() - aw, base.x() + r, base.y() - aw + r);
        path.lineTo(base.x() + aw, base.y());
        break;

    case Qt::BottomEdge:
        path.moveTo(base.x() - aw, base.y());
        path.lineTo(base.x(), base.y() + aw - r);
        path.quadTo(base.x(), base.y() + aw, base.x() + r, base.y() + aw - r);
        path.lineTo(base.x() + aw, base.y());
        break;

    case Qt::LeftEdge:
        path.moveTo(base.x(), base.y() - aw);
        path.lineTo(base.x() - aw + r, base.y());
        path.quadTo(base.x() - aw, base.y(), base.x() - aw + r, base.y() + r);
        path.lineTo(base.x(), base.y() + aw);
        break;

    case Qt::RightEdge:
        path.moveTo(base.x(), base.y() - aw);
        path.lineTo(base.x() + aw - r, base.y());
        path.quadTo(base.x() + aw, base.y(), base.x() + aw - r, base.y() + r);
        path.lineTo(base.x(), base.y() + aw);
        break;
    }

    path.closeSubpath();
    return path;
}
