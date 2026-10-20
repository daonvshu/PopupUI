#include "shadowwidget.h"

#include <qevent.h>
#include <qmath.h>
#include <qpainter.h>
#include <qstyleoption.h>
#include <qpainterpath.h>
#include <qdebug.h>

// Qt internal function (qtbase/src/widgets/effects/qpixmapfilter.cpp)
extern void qt_blurImage(QPainter* p, QImage& blurImage, qreal radius, bool quality, bool alphaOnly, int transposed);

POPUPUI_BEGIN_NAMESPACE

ShadowWidget::ShadowWidget(QWidget* parent)
    : QWidget(parent)
{
    setContentsMargins(shadowMargins());
}

void ShadowWidget::showArrow(bool show) {
    isShowArrow = show;
}

void ShadowWidget::resizeContent(int width, int height) {
    resize(outerSizeForContent(QSize(width, height)));
}

void ShadowWidget::setContentFixedSize(int width, int height) {
    setFixedSize(outerSizeForContent(QSize(width, height)));
}

void ShadowWidget::setContentFixedWidth(int width) {
    setFixedWidth(outerSizeForContent(QSize(width, contentSizeForOuter(size()).height())).width());
}

void ShadowWidget::setContentFixedHeight(int height) {
    setFixedHeight(outerSizeForContent(QSize(contentSizeForOuter(size()).width(), height)).height());
}

void ShadowWidget::setShadowRadius(int radius) {
    const QMargins oldMargins = expandedShadowMargins;
    shadowRadius = qMax(0, radius);
    shadowImage = QImage();
    const QMargins newMargins = shadowMargins();
    setContentsMargins(newMargins);
    if (shadowSizeExpanded) {
        expandedShadowMargins = newMargins;
        adjustSizeForShadowMargins(oldMargins, newMargins);
    }
    repaint();
}

void ShadowWidget::setAutoExpandShadowSize(bool enabled) {
    autoExpandShadowSize = enabled;
}

void ShadowWidget::setAutoAdjustShadowPosition(bool enabled) {
    autoAdjustShadowPosition = enabled;
}

bool ShadowWidget::event(QEvent* event) {
    if (event && event->type() == QEvent::Show) {
        ensureShadowSizeExpanded();
    }
    return QWidget::event(event);
}

void ShadowWidget::paintEvent(QPaintEvent*) {
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
    opt.initFrom(this);
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

    const qreal r  = 2.0;                    // Corner radius
    const qreal aw = 7.0;                    // Arrow half-height/half-width
    const qreal offset = shadowRadius + shadowCornerRadius + 12; // Clearance from the corners
    const qreal sr = shadowRadius;

    QPainterPath path;
    QPointF base; // Arrow base point (midpoint of its base edge)

    // ---- Step 1: Calculate the base point for the arrow edge ----
    switch (arrowEdgeAlign) {
    case Qt::TopEdge:
        // Top-edge arrow: fix the vertical position at shadowRadius
        base.setY(sr);
        // Horizontal position (left, center, right)
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

    // ---- Step 2: Draw the arrow path for the edge direction ----
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

QMargins ShadowWidget::shadowMargins() const {
    return {
        shadowEdges & Qt::LeftEdge ? shadowRadius : 0,
        shadowEdges & Qt::TopEdge ? shadowRadius : 0,
        shadowEdges & Qt::RightEdge ? shadowRadius : 0,
        shadowEdges & Qt::BottomEdge ? shadowRadius : 0,
    };
}

QSize ShadowWidget::outerSizeForContent(const QSize& contentSize) const {
    if (!shadowSizeExpanded) {
        return contentSize;
    }
    return contentSize + QSize(
        expandedShadowMargins.left() + expandedShadowMargins.right(),
        expandedShadowMargins.top() + expandedShadowMargins.bottom()
    );
}

QSize ShadowWidget::contentSizeForOuter(const QSize& outerSize) const {
    if (!shadowSizeExpanded) {
        return outerSize;
    }
    return outerSize - QSize(
        expandedShadowMargins.left() + expandedShadowMargins.right(),
        expandedShadowMargins.top() + expandedShadowMargins.bottom()
    );
}

void ShadowWidget::adjustSizeForShadowMargins(const QMargins& oldMargins, const QMargins& newMargins) {
    const QSize sizeDelta(
        newMargins.left() + newMargins.right() - oldMargins.left() - oldMargins.right(),
        newMargins.top() + newMargins.bottom() - oldMargins.top() - oldMargins.bottom()
    );
    const QPoint positionDelta(
        newMargins.left() - oldMargins.left(),
        newMargins.top() - oldMargins.top()
    );
    if (sizeDelta.isNull() && positionDelta.isNull()) {
        return;
    }

    const QSize currentSize = size();
    const QPoint currentPos = pos();
    const QSize currentMinimum = minimumSize();
    const QSize currentMaximum = maximumSize();
    const QSize newMinimum(
        qBound(0, currentMinimum.width() + sizeDelta.width(), QWIDGETSIZE_MAX),
        qBound(0, currentMinimum.height() + sizeDelta.height(), QWIDGETSIZE_MAX)
    );
    const QSize newMaximum(
        currentMaximum.width() == QWIDGETSIZE_MAX
            ? QWIDGETSIZE_MAX
            : qBound(newMinimum.width(), currentMaximum.width() + sizeDelta.width(), QWIDGETSIZE_MAX),
        currentMaximum.height() == QWIDGETSIZE_MAX
            ? QWIDGETSIZE_MAX
            : qBound(newMinimum.height(), currentMaximum.height() + sizeDelta.height(), QWIDGETSIZE_MAX)
    );

    setMinimumSize(newMinimum);
    setMaximumSize(newMaximum);
    resize(
        qMax(newMinimum.width(), currentSize.width() + sizeDelta.width()),
        qMax(newMinimum.height(), currentSize.height() + sizeDelta.height())
    );
    if (autoAdjustShadowPosition) {
        move(currentPos - positionDelta);
    }
}

void ShadowWidget::ensureShadowSizeExpanded() {
    if (shadowSizeExpanded || !autoExpandShadowSize) {
        return;
    }

    expandedShadowMargins = shadowMargins();
    shadowSizeExpanded = true;
    adjustSizeForShadowMargins(QMargins(), expandedShadowMargins);
}

POPUPUI_END_NAMESPACE
