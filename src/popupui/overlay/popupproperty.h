#pragma once

#include <popupui/comm/global.h>
#include <qwidget.h>

POPUPUI_BEGIN_NAMESPACE

/**
 * @brief Represents a property that can be used to customize the appearance or behavior of a popup.
 *
 * This class provides a way to define and manage properties for popups, such as position, size, or visibility.
 */
class POPUPUI_EXPORT PopupProperty {
    /**
     * @brief Indicates whether the popup can be closed by clicking outside of it.
     *
     * This boolean property determines if the popup should respond to a click
     * outside its boundaries by closing itself. When set to `true`, the popup will
     * close when a user clicks outside of it; otherwise, it remains open regardless
     * of external clicks.
     */
    bool closable = false;

    /**
     * @brief Initializes the color to Qt::transparent, which is used as the default mask color in PopupProperty.
     *
     * This initialization sets the QColor to a transparent value, meaning that when this color
     * is used (e.g., as a mask), it will not be visible. This color represents the current popup layer's
     * mask color and does not affect the visibility of other layers. However, it will hide the base mask
     * layer if set to a non-transparent color.
     *
     * The color can later be modified using the `setMaskColor` method of the PopupProperty struct.
     */
    QColor color = Qt::transparent;

    /**
     * @brief Determines whether the mouse events should be intercepted by the dialog mask.
     *
     * When set to true, the dialog mask will intercept and handle mouse events. If set to false,
     * the underlying widgets can receive and process mouse events through the transparent mask.
     *
     * @note This property is used in conjunction with the DialogMask's event filter to control
     * the transparency of the mask for mouse events.
     */
    bool interceptMouse = true;

    /**
     * @brief Determines whether the dialog should be deleted when it is closed.
     *
     * When set to true, the dialog widget will be automatically deleted from memory
     * upon being closed. This is useful for managing the lifecycle of temporary or
     * one-time use dialogs without needing to manually handle their deletion.
     *
     * @note This property is part of the PopupProperty struct and can be modified
     * using the setDeleteOnClose method.
     */
    bool deleteOnClose = true;

    /**
     * @brief Pointer to a QWidget that defines the draggable area of the popup.
     *
     * This member variable is used to specify which widget within the popup can be
     * dragged. If set, the user can click and drag this widget to move the entire
     * popup. The default value is `nullptr`, indicating no specific widget is
     * designated as draggable.
     *
     * To enable dragging, set this pointer to a valid QWidget instance. The dragging
     * behavior can be further customized using other properties in the PopupProperty
     * struct, such as `boundedDrag` to restrict the dragging within certain bounds.
     */
    QWidget* draggableWidget = nullptr;

    /**
     * @brief Indicates whether the drag operation is bounded within the parent widget.
     *
     * When set to true, dragging the dialog will be confined within the boundaries of its parent widget.
     * If false, the dialog can be dragged freely without any boundary constraints.
     */
    bool boundedDrag = true;

    /**
     * @brief Controls the visibility of the base mask layer.
     *
     * This boolean flag determines whether the base mask layer is visible or not. The base mask
     * is a background layer that can be used to dim out the area behind the dialog, making it stand
     * out more prominently.
     *
     * @note This property can be modified using the `showBaseMaskLayer` method of the PopupProperty class.
     */
    bool baseMaskVisible = true;

    /**
     * @brief Specifies the alignment of the popup relative to its target widget.
     *
     * This property is used in conjunction with `edgeAlign` to determine how the popup should
     * be aligned when it is displayed. The default value is set to `Qt::AlignHCenter`, which
     * centers the popup horizontally. Other possible values include `Qt::AlignLeft` and
     * `Qt::AlignRight` for horizontal alignment, and `Qt::AlignTop`, `Qt::AlignVCenter`, and
     * `Qt::AlignBottom` for vertical alignment.
     *
     * - When `edgeAlign` is set to `Qt::TopEdge` or `Qt::BottomEdge`, `posAlign` should be used
     *   to specify the horizontal alignment of the popup. For example, if `edgeAlign` is
     *   `Qt::TopEdge` and `posAlign` is `Qt::AlignLeft`, the left edge of the popup will be
     *   aligned with the top edge of the target widget.
     *
     * - When `edgeAlign` is set to `Qt::LeftEdge` or `Qt::RightEdge`, `posAlign` should be used
     *   to specify the vertical alignment of the popup. For example, if `edgeAlign` is
     *   `Qt::LeftEdge` and `posAlign` is `Qt::AlignTop`, the top edge of the popup will be
     *   aligned with the left edge of the target widget.
     *
     * @note This property is part of the PopupProperty struct and can be modified using the
     * alignToWidget method, among others, to adjust the popup's position relative to a target widget.
     */
    Qt::Alignment posAlign = Qt::AlignHCenter;

    /**
     * @brief Specifies the edge of the widget to which the popup will be aligned.
     *
     * This value determines along which edge of the target widget (or screen) the
     * popup dialog will align itself. The alignment can be one of the Qt::Edge values,
     * such as Qt::TopEdge, Qt::BottomEdge, Qt::LeftEdge, or Qt::RightEdge.
     * By default, it is set to Qt::TopEdge, meaning the popup will align to the top
     * edge of the specified target.
     *
     * @see Qt::Edge for a list of possible edge alignments.
     */
    Qt::Edge edgeAlign = Qt::TopEdge;

    /**
     * Pointer to the target widget to which the popup will be aligned.
     *
     * When set, this property specifies a widget that the popup will align itself to based on the alignment
     * properties such as `alignToPos`, `edgeAlign`, and `posAlign`. If this pointer is `nullptr`, the popup
     * will not align to any specific widget.
     *
     * The actual position of the popup relative to the target widget can be further adjusted using the
     * `alignOffset` property.
     *
     * @see alignToPos
     * @see edgeAlign
     * @see posAlign
     * @see alignOffset
     */
    QWidget* alignToTarget = nullptr;

    /**
     * @brief Specifies the alignment of the popup relative to its target widget.
     *
     * This member variable is used to determine the position of the popup in relation
     * to the widget it is aligned with. The default value aligns the popup to the
     * bottom-left corner of the target widget.
     *
     * @note This alignment can be customized using the `alignToWidget` method of the
     * PopupProperty struct.
     */
    Qt::Alignment alignToPos = Qt::AlignLeft | Qt::AlignBottom;

    /**
     * @brief A function object that provides the position to which the widget should be aligned.
     *
     * This std::function is used to dynamically calculate the alignment position of a widget. It
     * returns a QPoint representing the desired position. This is particularly useful when the
     * target position needs to be determined based on dynamic or complex criteria, such as
     * the current state of the application or user interactions.
     *
     * If this function is set, it will override the static alignment settings and use the
     * provided position for alignment. The function should return a valid QPoint that
     * represents the position relative to the global coordinate system.
     */
    std::function<QPoint()> alignToPosProvider;

    /**
     * @brief Represents an offset to be applied when aligning the popup to a target widget.
     *
     * This point is used in conjunction with other alignment properties to fine-tune the position
     * of the popup relative to the target widget. The offset is added to the calculated position
     * based on the alignment settings.
     */
    QPoint alignOffset;

    /**
     * @brief The aspect ratio for the width of the widget, used to maintain a specific width relative to its parent.
     *
     * A value of -1 indicates that no specific width aspect ratio is set. When a positive value is assigned,
     * it will be used to calculate the width of the associated widget relative to its parent's width during
     * resize operations. This is particularly useful for maintaining a consistent visual layout across different
     * screen sizes or when the parent widget's size changes.
     *
     * @note For the aspect ratio to take effect, ensure that both `widthAspectRatio` and `heightAspectRatio` are
     * set appropriately using the `keepAspectRatio` method of the PopupProperty struct.
     */
    double widthAspectRatio = -1;

    /**
     * @brief The aspect ratio for the height of a widget relative to its parent.
     *
     * This value is used to maintain a proportional height to the parent widget's height. A value
     * of -1 indicates that no specific aspect ratio should be applied, and the widget will not
     * automatically adjust its height based on the parent's dimensions.
     *
     * To set both width and height aspect ratios, use the `keepAspectRatio` method of the PopupProperty struct.
     */
    double heightAspectRatio = -1;

public:
    /**
     * @brief Enables or disables the close on click outside feature for the popup.
     *
     * This method sets whether the popup should be closed when the user clicks
     * outside of it. By default, this feature is enabled if no argument is provided.
     *
     * @param enabled A boolean value indicating whether to enable or disable the
     *                close on click outside feature. If true (default), the popup
     *                will close when clicked outside; if false, it will not.
     * @return A reference to the current PopupProperty instance, allowing for
     *         method chaining.
     */
    PopupProperty& closeOnClickOutside(bool enabled = true) {
        this->closable = enabled;
        return *this;
    }

    /**
     * @brief Sets the mask color of the PopupProperty.
     *
     * This function updates the color used for the mask and hides the base mask layer. It returns a reference to the current object, allowing for method chaining.
     *
     * @param maskColor The QColor to be set as the new mask color.
     * @return A reference to the updated PopupProperty object.
     */
    PopupProperty& setMaskColor(const QColor& maskColor) {
        this->color = maskColor;
        showBaseMaskLayer(false);
        return *this;
    }

    /**
     * @brief Enables or disables the transparency of mouse events for the popup.
     *
     * When enabled, mouse events will pass through the popup to the underlying widgets.
     * When disabled, the popup will intercept and handle mouse events.
     *
     * @param enabled If true, enables transparent mouse events. If false, disables them.
     *                Default is true.
     * @return Reference to the current PopupProperty object, allowing method chaining.
     */
    PopupProperty& transparentMouseEvent(bool enabled = true) {
        this->interceptMouse = !enabled;
        return *this;
    }

    /**
     * @brief Sets whether the popup should be deleted when it is closed.
     *
     * This method allows for setting a boolean value that determines if the popup
     * should automatically delete itself upon being closed. If `enabled` is set to
     * true, the popup will be deleted after it is closed; otherwise, it will not.
     *
     * @param enabled A boolean indicating if the popup should be deleted on close.
     *                Defaults to true.
     * @return A reference to the current PopupProperty object, allowing for method chaining.
     */
    PopupProperty& setDeleteOnClose(bool enabled = true) {
        this->deleteOnClose = enabled;
        return *this;
    }

    /**
     * @brief Sets the widget that can be used to drag the popup and whether the dragging is bounded.
     *
     * This function configures the popup's draggable area, which allows the user to move the popup by
     * dragging the specified widget. The `bounded` parameter controls whether the dragging is limited
     * within the bounds of the parent widget.
     *
     * @param draggableAreaWidget The widget that will be used as the draggable area for the popup.
     * @param bounded A boolean indicating if the dragging should be bounded within the parent widget.
     *                Default is true.
     * @return A reference to the current PopupProperty object, allowing for method chaining.
     */
    PopupProperty& setDraggableArea(QWidget* draggableAreaWidget, bool bounded = true) {
        this->draggableWidget = draggableAreaWidget;
        this->boundedDrag = bounded;
        return *this;
    }

    /**
     * @brief Sets the visibility of the base mask layer.
     *
     * This function allows setting whether the base mask layer is visible or not. The base mask
     * layer can be used to provide a visual indication behind the popup, often for dimming the
     * background or highlighting the popup area.
     *
     * @param show A boolean value indicating if the base mask layer should be visible. Defaults to true.
     * @return A reference to the current PopupProperty object, allowing method chaining.
     */
    PopupProperty& showBaseMaskLayer(bool show = true) {
        this->baseMaskVisible = show;
        return *this;
    }

    /**
     * Aligns the popup to a specified widget.
     *
     * @param target The widget to align the popup with.
     * @param targetPos The alignment position relative to the target widget.
     * @param edgeAlign The edge of the popup to align. Defaults to Qt::TopEdge.
     * @param posAlign The alignment of the popup's position. Defaults to Qt::AlignHCenter.
     * @param offset The offset from the aligned position. Defaults to QPoint(0, 0).
     * @return A reference to the PopupProperty object for method chaining.
     */
    PopupProperty& alignToWidget(QWidget* target, Qt::Alignment targetPos, Qt::Edge edgeAlign = Qt::TopEdge, Qt::Alignment posAlign = Qt::AlignHCenter, const QPoint& offset = QPoint(0, 0)) {
        this->alignToTarget = target;
        this->alignToPos = targetPos;
        this->edgeAlign = edgeAlign;
        this->posAlign = posAlign;
        this->alignOffset = offset;
        return *this;
    }

    /**
     * Aligns the popup to a specified widget using a provided position provider function.
     *
     * @param target The widget to which the popup will be aligned.
     * @param targetPosProvider A function that provides the position relative to the target widget.
     * @param edgeAlign The edge of the target widget to align with. Default is Qt::TopEdge.
     * @param posAlign The alignment of the popup relative to the target's edge. Default is Qt::AlignHCenter.
     * @param offset An additional offset to apply to the calculated position. Default is (0, 0).
     * @return A reference to the PopupProperty object for method chaining.
     */
    PopupProperty& alignToWidget(QWidget* target, const std::function<QPoint()>& targetPosProvider, Qt::Edge edgeAlign = Qt::TopEdge, Qt::Alignment posAlign = Qt::AlignHCenter, const QPoint& offset = QPoint(0, 0)) {
        this->alignToTarget = target;
        this->alignToPosProvider = targetPosProvider;
        this->edgeAlign = edgeAlign;
        this->posAlign = posAlign;
        this->alignOffset = offset;
        return *this;
    }

    /**
     * @brief Sets the aspect ratio for the popup to keep.
     *
     * This method allows setting a specific width and height ratio that the popup
     * should maintain. This is useful for ensuring that the popup does not distort
     * its content when resized.
     *
     * @param widthRatio The desired width ratio.
     * @param heightRatio The desired height ratio.
     * @return A reference to the current PopupProperty object, allowing for method chaining.
     */
    PopupProperty& keepAspectRatio(double widthRatio, double heightRatio) {
        this->widthAspectRatio = widthRatio;
        this->heightAspectRatio = heightRatio;
        return *this;
    }
};

POPUPUI_END_NAMESPACE