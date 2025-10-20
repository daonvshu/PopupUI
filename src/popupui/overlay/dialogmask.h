#pragma once

#include <qwidget.h>
#include <popupui/comm/global.h>

POPUPUI_BEGIN_NAMESPACE

class DialogMask : public QWidget {
    Q_OBJECT

public:
    struct Property {
        bool closable = false;
        QColor color = Qt::transparent;
        bool interceptMouse = true;
        bool deleteOnClose = true;
        bool keepCenter = true;

        Property& closeOnClickOutside(bool enabled = true) {
            this->closable = enabled;
            return *this;
        }

        Property& setMaskColor(const QColor& maskColor) {
            this->color = maskColor;
            return *this;
        }

        Property& transparentMouseEvent(bool enabled = true) {
            this->interceptMouse = !enabled;
            return *this;
        }

        Property& setDeleteOnClose(bool enabled = true) {
            this->deleteOnClose = enabled;
            return *this;
        }

        Property& keepCenterOnResized(bool enabled = true) {
            this->keepCenter = enabled;
            return *this;
        }
    };

    explicit DialogMask(QWidget* parent, QWidget* dlg, const Property& props);

signals:
    void requestClose();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    QWidget* targetDlg;
    Property props;
};

POPUPUI_END_NAMESPACE